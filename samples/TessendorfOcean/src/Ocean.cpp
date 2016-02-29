#include "Ocean.h"

#include "math/Vector4.h"

#include <render/Buffer.h>
#include <render/ConstantBuffers.h>
#include <render/HardwareResourceCreator.h>
#include <render/RenderDevice.h>
#include <render/Shader.h>

using namespace Sketch3D;

#include <string>
using namespace std;

Ocean::Ocean(const int numberOfPoints, const float amplitude, const Vector2& windStrength, const float length, const shared_ptr<RenderDevice>& renderDevice)
    : m_RenderDevice(renderDevice)
    , numberOfPoints_(numberOfPoints)
    , amplitude_(amplitude)
    , wind_(windStrength)
    , length_(length)
    , gravity_(9.81f)
{
    windLength_ = wind_.Length();
    wind_.Normalize();

    // Create the vertices of the mesh
    size_t numVertices = numberOfPoints_ * numberOfPoints_;
    m_Vertices.reserve(numVertices);
    initialPositions_.resize(numVertices);

    Vector2 v;
    size_t idx = 0;
    for (int i = 0; i < numberOfPoints_; i++) {
        v.y = (i - numberOfPoints_ / 2.0f) * length_ / numberOfPoints_;

        for (int j = 0; j < numberOfPoints_; j++) {
            idx = i * numberOfPoints_ + j;
            v.x = (j - numberOfPoints_ / 2.0f) * length_ / numberOfPoints_;

            Vertex_Pos_Normal_t vertex;
            vertex.position = Vector3(v.x, 0.0f, v.y);
            initialPositions_[idx] = Vector2(vertex.position.x, vertex.position.z);
            vertex.normal = Vector3::UP;

            m_Vertices.push_back(vertex);

            // Compute the initial wave factors at the same time
            hTilde0_.push_back(ComputeHTilde0(i, j));
            hTilde0Conjugate_.push_back(ComputeHTilde0(-i, -j).Conjugate());
        }
    }

    // Compute the indices of the mesh
    int size = numberOfPoints_ - 1;
    vector<unsigned int> indices;
    indices.reserve(size * size * 6);
    idx = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            indices.push_back(i + j * numberOfPoints_);
            indices.push_back(i + (j + 1) * numberOfPoints_);
            indices.push_back(i + j * numberOfPoints_ + 1);

            indices.push_back(i + (j + 1) * numberOfPoints_);
            indices.push_back(i + (j + 1) * numberOfPoints_ + 1);
            indices.push_back(i + j * numberOfPoints_ + 1);
        }
    }

    m_VertexBuffer = m_RenderDevice->GetHardwareResourceCreator()->CreateVertexBuffer();
    m_VertexBuffer->Initialize((void*)&m_Vertices[0], true, false, VertexFormatType_t::Pos_Normal, m_Vertices.size());

    m_IndexBuffer = m_RenderDevice->GetHardwareResourceCreator()->CreateIndexBuffer();
    m_IndexBuffer->Initialize((void*)&indices[0], false, false, IndexFormat_t::INT_4_BYTES, indices.size());

    // Load the shader
    shared_ptr<VertexShader> vertexShader = m_RenderDevice->GetHardwareResourceCreator()->CreateVertexShader();
    shared_ptr<FragmentShader> fragmentShader = m_RenderDevice->GetHardwareResourceCreator()->CreateFragmentShader();
    vertexShader->InitializeFromFile("Shaders/TessendorfOcean/vert.hlsl");
    fragmentShader->InitializeFromFile("Shaders/TessendorfOcean/frag.hlsl");

    m_RenderDevice->SetVertexShader(vertexShader);
    m_RenderDevice->SetFragmentShader(fragmentShader);

    // Initialize the FFT specific members
    log_2_N_ = (size_t)(log(numberOfPoints_) / log(2));

    reversed_ = new size_t[numberOfPoints_];
    for (int i = 0; i < numberOfPoints_; i++) {
        reversed_[i] = Reverse(i);
    }

    size_t pow2 = 1;
    twiddleFactors_ = new Complex* [log_2_N_];
    for (size_t i = 0; i < log_2_N_; i++) {
        twiddleFactors_[i] = new Complex[pow2];
        
        for (size_t j = 0; j < pow2; j++) {
            twiddleFactors_[i][j] = ComputeTwiddleFactor(j, pow2 * 2);
        }

        pow2 *= 2;
    }

    c_height_[0] = new Complex[numberOfPoints_];
    c_height_[1] = new Complex[numberOfPoints_];
    c_slopex_[0] = new Complex[numberOfPoints_];
    c_slopex_[1] = new Complex[numberOfPoints_];
    c_slopez_[0] = new Complex[numberOfPoints_];
    c_slopez_[1] = new Complex[numberOfPoints_];
    c_dx_[0] = new Complex[numberOfPoints_];
    c_dx_[1] = new Complex[numberOfPoints_];
    c_dz_[0] = new Complex[numberOfPoints_];
    c_dz_[1] = new Complex[numberOfPoints_];

    hTildeHeight_ = new Complex[numVertices];
    hTildeSlopex_ = new Complex[numVertices];
    hTildeSlopez_ = new Complex[numVertices];
    hTildeDx_ = new Complex[numVertices];
    hTildeDz_ = new Complex[numVertices];

    m_DrawConstants = m_RenderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    m_OceanConstants = m_RenderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    m_DrawConstants->Initialize(nullptr, true, false, sizeof(DrawConstants_t));

    OceanConstants_t oceanConstants;
    oceanConstants.light_position = Vector4(0.0f, 10.0f, -16.0f);
    oceanConstants.ambient_color = Vector4(0.0f, 0.65f, 0.75f);
    oceanConstants.diffuse_color = Vector4(0.5f, 0.65f, 0.75f);
    oceanConstants.specular_color = Vector4(1.0f, 0.25f, 0.0f, 120.0f);
    oceanConstants.light_contribution =  Vector4(0.3f, 1.80f, 0.3f);

    m_OceanConstants->Initialize(&oceanConstants, false, false, sizeof(OceanConstants_t));
}

Ocean::~Ocean() {
    // delete oceanMaterial_;

    delete[] reversed_;
    for (size_t i = 0; i < log_2_N_; i++) {
        delete[] twiddleFactors_[i];
    }
    delete[] twiddleFactors_;
    delete[] c_height_[0];
    delete[] c_height_[1];
    delete[] c_slopex_[0];
    delete[] c_slopex_[1];
    delete[] c_slopez_[0];
    delete[] c_slopez_[1];
    delete[] c_dx_[0];
    delete[] c_dx_[1];
    delete[] c_dz_[0];
    delete[] c_dz_[1];
}

void Ocean::EvaluateWaves(double t) {
    Vector2 k;
    float kLength;
    size_t index;

    for (int i = 0; i < numberOfPoints_; i++) {
        k.y = PI * (2.0f * i - numberOfPoints_) / length_;

        for (int j = 0; j < numberOfPoints_; j++) {
            k.x = PI * (2.0f * j - numberOfPoints_) / length_;
            kLength = k.Length();
            index = i * numberOfPoints_ + j;

            hTildeHeight_[index] = ComputeHTilde(t, i, j);
            hTildeSlopex_[index] = hTildeHeight_[index] * Complex(0.0f, k.x);
            hTildeSlopez_[index] = hTildeHeight_[index] * Complex(0.0f, k.y);

            if (kLength < EPSILON) {
                hTildeDx_[index] = Complex(0.0f, 0.0f);
                hTildeDz_[index] = Complex(0.0f, 0.0f);
            } else {
                hTildeDx_[index] = hTildeHeight_[index] * Complex(0.0f, -k.x/kLength);
                hTildeDz_[index] = hTildeHeight_[index] * Complex(0.0f, -k.y/kLength);
            }
        }
    }

    // Multi thread the fft passes
    thread heightThread(&Ocean::FftPassHeight, this);
    thread slopexThread(&Ocean::FftPassSlopex, this);
    thread slopezThread(&Ocean::FftPassSlopez, this);
    thread dxThread(&Ocean::FftPassDx, this);
    thread dzThread(&Ocean::FftPassDz, this);

    heightThread.join();
    slopexThread.join();
    slopezThread.join();
    dxThread.join();
    dzThread.join();

    // Displace the vertices
    float lambda = -1.0f;
    const float signs[] = { 1.0f, -1.0f };
    float sign;

    for (int i = 0; i < numberOfPoints_; i++) {
        for (int j = 0; j < numberOfPoints_; j++) {
            index = i * numberOfPoints_ + j;

            sign = signs[(i + j) & 1];

            hTildeHeight_[index] *= sign;
            m_Vertices[index].position.y = hTildeHeight_[index].a;

            hTildeDx_[index] *= sign;
            hTildeDz_[index] *= sign;
            m_Vertices[index].position.x = initialPositions_[index].x + hTildeDx_[index].a * lambda;
            m_Vertices[index].position.z = initialPositions_[index].y + hTildeDz_[index].a * lambda;

            hTildeSlopex_[index] *= sign;
            hTildeSlopez_[index] *= sign;
            m_Vertices[index].normal = Vector3(-hTildeSlopex_[index].a, 1.0f, -hTildeSlopez_[index].a).Normalized();
        }
    }
}

void Ocean::Render(const Matrix4x4& viewMatrix) {
    void* data = m_VertexBuffer->Map(MapFlag_t::WRITE_DISCARD);

    memcpy(data, &m_Vertices[0], m_Vertices.size() * sizeof(m_Vertices[0]));

    m_VertexBuffer->Unmap();

    /*
    oceanMaterial_->SetUniformVector3("light_position", Vector3(0.0f, 10.0f, -16.0f));
    oceanMaterial_->SetUniformVector3("ambient_color", Vector3(0.0f, 0.65f, 0.75f));
    oceanMaterial_->SetUniformVector3("diffuse_color", Vector3(0.5f, 0.65f, 0.75f));
    oceanMaterial_->SetUniformVector4("specular_color", Vector4(1.0f, 0.25f, 0.0f, 120.0f));
    oceanMaterial_->SetUniformVector3("light_contribution", Vector3(0.3f, 1.80f, 0.3f));
    */

    data = m_DrawConstants->Map(MapFlag_t::WRITE_DISCARD);
    DrawConstants_t* drawConstants = (DrawConstants_t*)data;

    drawConstants->modelViewMatrix = viewMatrix;
    drawConstants->modelViewProjectionMatrix = m_RenderDevice->CalculatePerspectiveProjectionFOVRightHanded(60.0f, 1024.0f / 768.0f, 1.0f, 1000.0f) * viewMatrix;

    m_DrawConstants->Unmap();

    m_RenderDevice->SetVertexShaderConstantBuffer(m_DrawConstants, 0);
    m_RenderDevice->SetVertexShaderConstantBuffer(m_OceanConstants, 1);
    m_RenderDevice->SetFragmentShaderConstantBuffer(m_OceanConstants, 1);
    m_RenderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, m_VertexBuffer, m_IndexBuffer, 0, 0);
}

Complex Ocean::ComputeHTilde0(int i, int j) const {
    Complex r = GaussianRandomVariable();
    return r * sqrt(Phillips(i, j) / 2.0f);
}

Complex Ocean::GaussianRandomVariable() const {
    float x1, x2, w;
    do {
        x1 = 2.0f * UniformRandomVariable() - 1.0f;
        x2 = 2.0f * UniformRandomVariable() - 1.0f;
        w = x1 * x1 + x2 * x2;
    } while (w >= 1.0f);
    w = sqrt((-2.0f * log(w)) / w);
    return Complex(x1 * w, x2 * w);
}

float Ocean::UniformRandomVariable() const {
    return (float)rand()/RAND_MAX;
}

float Ocean::Phillips(int i, int j) const {
    Vector2 k(PI * (2.0f * i - numberOfPoints_) / length_, PI * (2.0f * j - numberOfPoints_) / length_);
    float kLength = k.Length();

    if (kLength < EPSILON) {
        return 0.0f;
    }

    float kLength2 = kLength * kLength;
    float kLength4 = kLength2 * kLength2;

    k.x /= kLength;
    k.y /= kLength;
    float kDotW = k.Dot(wind_);
    float kDotW2 = kDotW * kDotW * kDotW * kDotW * kDotW * kDotW;

    float L = windLength_ * windLength_ / gravity_;
    float L2 = L * L;

    float damping = 0.001f;
    float l2 = L2 * damping * damping;

    return amplitude_ * exp(-1.0f / (kLength2 * L2)) / kLength4 * kDotW2 * exp(-kLength2 * l2);
}

size_t Ocean::Reverse(int i) const {
    size_t res = 0;
    for (int j = 0; j < (int)log_2_N_; j++) {
        res = (res << 1) + (i & 1);
        i >>= 1;
    }

    return res;
}

Complex Ocean::ComputeTwiddleFactor(int x, int dimension) const {
    float coeff = PI * 2 * x / dimension;
    return Complex(cos(coeff), sin(coeff));
}

Complex Ocean::ComputeHTilde(double t, int i, int j) const {
    size_t index = i * numberOfPoints_ + j;

    float omegat = Dispersion(i, j) * (float)t;

    float cos_ = cosf(omegat);
    float sin_ = sinf(omegat);

    Complex c0(cos_,  sin_);
    Complex c1(cos_, -sin_);

    return hTilde0_[index] * c0 + hTilde0Conjugate_[index] * c1;
}

float Ocean::Dispersion(int i, int j) const {
    const float w_0 = PI / 100.0f;
    Vector2 k(PI * (2.0f * i - numberOfPoints_) / length_, PI * (2.0f * j - numberOfPoints_) / length_);
    return floor(sqrt(gravity_ * k.Length()) / w_0) * w_0;
}

void Ocean::Fft(Complex* input, Complex* output, size_t stride, size_t offset, Complex* c[]) {
    size_t which = 0;
    for (int i = 0; i < numberOfPoints_; i++) {
        size_t r = reversed_[i] * stride + offset;
        c[which][i] = input[r];
    }

    int loops = numberOfPoints_ >> 1;
    size_t size = 1 << 1;
    size_t size_over_2 = 1;
    size_t w_ = 0;

    for (size_t i = 1; i <= log_2_N_; i++) {
        which ^= 1;

        for (size_t j = 0; j < (size_t)loops; j++) {
            for (size_t k = 0; k < size_over_2; k++) {
                c[which][size * j + k] = c[which^1][size * j + k] + c[which^1][size * j + size_over_2 + k] * twiddleFactors_[w_][k];
            }

            for (size_t k = size_over_2; k < size; k++) {
                c[which][size * j + k] = c[which^1][size * j - size_over_2 + k] - c[which^1][size * j + k] * twiddleFactors_[w_][k - size_over_2];
            }
        }

        loops >>= 1;
        size <<= 1;
        size_over_2 <<= 1;
        w_++;
    }

    for (int i = 0; i < numberOfPoints_; i++) {
        output[i * stride + offset] = c[which][i];
    }
}

void Ocean::FftPassHeight() {
    // Horizontal pass
    for (int i = 0; i < numberOfPoints_; i++) {
        int coef = i * numberOfPoints_;

        Fft(hTildeHeight_, hTildeHeight_, 1, coef, c_height_);
    }

    // Vertical pass
    for (int j = 0; j < numberOfPoints_; j++) {
        Fft(hTildeHeight_, hTildeHeight_, numberOfPoints_, j, c_height_);
    }
}

void Ocean::FftPassSlopex() {
    // Horizontal pass
    for (int i = 0; i < numberOfPoints_; i++) {
        int coef = i * numberOfPoints_;

        Fft(hTildeSlopex_, hTildeSlopex_, 1, coef, c_slopex_);
    }

    // Vertical pass
    for (int j = 0; j < numberOfPoints_; j++) {
        Fft(hTildeSlopex_, hTildeSlopex_, numberOfPoints_, j, c_slopex_);
    }
}

void Ocean::FftPassSlopez() {
    // Horizontal pass
    for (int i = 0; i < numberOfPoints_; i++) {
        int coef = i * numberOfPoints_;

        Fft(hTildeSlopez_, hTildeSlopez_, 1, coef, c_slopez_);
    }

    // Vertical pass
    for (int j = 0; j < numberOfPoints_; j++) {
        Fft(hTildeSlopez_, hTildeSlopez_, numberOfPoints_, j, c_slopez_);
    }
}

void Ocean::FftPassDx() {
    // Horizontal pass
    for (int i = 0; i < numberOfPoints_; i++) {
        int coef = i *numberOfPoints_;

        Fft(hTildeDx_, hTildeDx_, 1, coef, c_dx_);
    }

    // Vertical pass
    for (int j = 0; j < numberOfPoints_; j++) {
        Fft(hTildeDx_, hTildeDx_, numberOfPoints_, j, c_dx_);
    }
}

void Ocean::FftPassDz() {
    // Horizontal pass
    for (int i = 0; i < numberOfPoints_; i++) {
        int coef = i * numberOfPoints_;

        Fft(hTildeDz_, hTildeDz_, 1, coef, c_dz_);
    }

    // Vertical pass
    for (int j = 0; j < numberOfPoints_; j++) {
        Fft(hTildeDz_, hTildeDz_, numberOfPoints_, j, c_dz_);
    }
}