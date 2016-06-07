#include "framework/SimpleObject.h"

#include "framework/Material.h"
#include "framework/MaterialCodeGenerator.h"
#include "framework/Mesh.h"
#include "framework/SubMesh.h"

#include "render/Buffer.h"
#include "render/HardwareResourceCreator.h"
#include "render/RenderDevice.h"
#include "render/Shader.h"

namespace Sketch3D
{

bool s_IsInitialized = false;

shared_ptr<Mesh> g_FullscreenQuad;

shared_ptr<Mesh> g_UnitCubeMesh;
shared_ptr<Mesh> g_UnitPlaneMesh;
shared_ptr<Mesh> g_UnitSphereMesh;

void CreateFullscreenQuad(const shared_ptr<RenderDevice>& renderDevice);
void CreateUnitCube(const shared_ptr<RenderDevice>& renderDevice, const shared_ptr<Material>& material);
void CreateUnitPlane(const shared_ptr<RenderDevice>& renderDevice, const shared_ptr<Material>& material);
void CreateUnitSphere(const shared_ptr<RenderDevice>& renderDevice, const shared_ptr<Material>& material);

void InitializeSimpleObjects(const shared_ptr<RenderDevice>& renderDevice, MaterialCodeGenerator* materialCodeGenerator)
{
    if (renderDevice.get() == nullptr)
    {
        return;
    }

    if (s_IsInitialized)
    {
        return;
    }

    s_IsInitialized = true;

    shared_ptr<Material> material(new Material(renderDevice));
    material->Initialize(VertexFormatType_t::Pos_Normal, materialCodeGenerator);

    CreateFullscreenQuad(renderDevice);
    CreateUnitCube(renderDevice, material);
	CreateUnitPlane(renderDevice, material);
    CreateUnitSphere(renderDevice, material);
}

void CreateFullscreenQuad(const shared_ptr<RenderDevice>& renderDevice)
{
    g_FullscreenQuad.reset(new Mesh);

    Vertex_Pos_t vertices[4];
    vertices[0].position = Vector3(-1.0f, -1.0f, 0.0f);
    vertices[1].position = Vector3(-1.0f,  1.0f, 0.0f);
    vertices[2].position = Vector3( 1.0f,  1.0f, 0.0f);
    vertices[3].position = Vector3( 1.0f, -1.0f, 0.0f);

    unsigned short indices[6];
    indices[0] = 0; indices[1] = 2; indices[2] = 1;
    indices[3] = 0; indices[4] = 3; indices[5] = 2;

    shared_ptr<VertexBuffer> vertexBuffer = renderDevice->GetHardwareResourceCreator()->CreateVertexBuffer();
    shared_ptr<IndexBuffer> indexBuffer = renderDevice->GetHardwareResourceCreator()->CreateIndexBuffer();
    vertexBuffer->Initialize(vertices, false, false, VertexFormatType_t::Pos, 4);
    indexBuffer->Initialize(indices, false, false, IndexFormat_t::INT_2_BYTES, 6);

    string vertexShaderCode =
        "struct VS_INPUT {"
            "float3 in_vertex : POSITION;"
        "};"

        "struct VS_OUTPUT {"
            "float4 position : SV_POSITION;"
            "float2 uv : TEXCOORD;"
        "};"

        "VS_OUTPUT main(VS_INPUT input) {"
            "VS_OUTPUT output;"
            "output.position = float4(input.in_vertex.xy, 0.0, 1.0);"
            "output.uv = input.in_vertex.xy * 0.5 + 0.5;"
            "return output;"
        "}";

    shared_ptr<VertexShader> vertexShader = renderDevice->GetHardwareResourceCreator()->CreateVertexShader();
    vertexShader->InitializeFromSource(vertexShaderCode);

    shared_ptr<Material> material(new Material(renderDevice));
    material->SetVertexShader(vertexShader);

    shared_ptr<SubMesh> subMesh(new SubMesh);
    subMesh->SetVertexBuffer(vertexBuffer);
    subMesh->SetIndexBuffer(indexBuffer);
    subMesh->SetMaterial(material);

    g_FullscreenQuad->AddSubMesh(subMesh);
}

void CreateUnitCube(const shared_ptr<RenderDevice>& renderDevice, const shared_ptr<Material>& material)
{
    g_UnitCubeMesh.reset(new Mesh);

    Vertex_Pos_Normal_t vertices[8];
    vertices[0].position = Vector3(-0.5f, -0.5f,  0.5f); vertices[0].normal = Vector3(-0.5773503f, -0.5773503f,  0.5773503f);
    vertices[1].position = Vector3(-0.5f,  0.5f,  0.5f); vertices[1].normal = Vector3(-0.5773503f,  0.5773503f,  0.5773503f);
    vertices[2].position = Vector3( 0.5f,  0.5f,  0.5f); vertices[2].normal = Vector3( 0.5773503f,  0.5773503f,  0.5773503f);
    vertices[3].position = Vector3( 0.5f, -0.5f,  0.5f); vertices[3].normal = Vector3( 0.5773503f, -0.5773503f,  0.5773503f);
    vertices[4].position = Vector3(-0.5f, -0.5f, -0.5f); vertices[4].normal = Vector3(-0.5773503f, -0.5773503f, -0.5773503f);
    vertices[5].position = Vector3(-0.5f,  0.5f, -0.5f); vertices[5].normal = Vector3(-0.5773503f,  0.5773503f, -0.5773503f);
    vertices[6].position = Vector3( 0.5f,  0.5f, -0.5f); vertices[6].normal = Vector3( 0.5773503f,  0.5773503f, -0.5773503f);
    vertices[7].position = Vector3( 0.5f, -0.5f, -0.5f); vertices[7].normal = Vector3( 0.5773503f, -0.5773503f, -0.5773503f);

    unsigned short indices[36];
    indices[0]  = 0; indices[1]  = 2; indices[2]  = 1; indices[3]  = 0; indices[4]  = 3; indices[5]  = 2; // Front
    indices[6]  = 3; indices[7]  = 6; indices[8]  = 2; indices[9]  = 3; indices[10] = 7; indices[11] = 6; // Right
    indices[12] = 7; indices[13] = 5; indices[14] = 6; indices[15] = 7; indices[16] = 4; indices[17] = 5; // Back
    indices[18] = 4; indices[19] = 1; indices[20] = 5; indices[21] = 4; indices[22] = 0; indices[23] = 1; // Left
    indices[24] = 1; indices[25] = 6; indices[26] = 5; indices[27] = 1; indices[28] = 2; indices[29] = 6; // Top
    indices[30] = 4; indices[31] = 3; indices[32] = 0; indices[33] = 4; indices[34] = 7; indices[35] = 3; // Bottom

    shared_ptr<VertexBuffer> vertexBuffer = renderDevice->GetHardwareResourceCreator()->CreateVertexBuffer();
    shared_ptr<IndexBuffer> indexBuffer = renderDevice->GetHardwareResourceCreator()->CreateIndexBuffer();
    vertexBuffer->Initialize(vertices, false, false, VertexFormatType_t::Pos_Color, 8);
    indexBuffer->Initialize(indices, false, false, IndexFormat_t::INT_2_BYTES, 36);

    shared_ptr<SubMesh> subMesh(new SubMesh);
    subMesh->SetVertexBuffer(vertexBuffer);
    subMesh->SetIndexBuffer(indexBuffer);
    subMesh->SetMaterial(material);

    g_UnitCubeMesh->AddSubMesh(subMesh);
}

void CreateUnitPlane(const shared_ptr<RenderDevice>& renderDevice, const shared_ptr<Material>& Material)
{
	g_UnitPlaneMesh.reset(new Mesh);

	Vertex_Pos_UV_Normal_Tangent_t vertices[4];
	vertices[0].position = Vector3(-0.5f, 0.0f,  0.5f); vertices[0].uv = Vector2(0.0f, 0.0f); vertices[0].normal = Vector3::UP; vertices[0].tangent = Vector3::RIGHT;
	vertices[1].position = Vector3(-0.5f, 0.0f, -0.5f); vertices[1].uv = Vector2(0.0f, 1.0f); vertices[1].normal = Vector3::UP; vertices[1].tangent = Vector3::RIGHT;
	vertices[2].position = Vector3( 0.5f, 0.0f, -0.5f); vertices[2].uv = Vector2(1.0f, 1.0f); vertices[2].normal = Vector3::UP; vertices[2].tangent = Vector3::RIGHT;
	vertices[3].position = Vector3( 0.5f, 0.0f,  0.5f); vertices[3].uv = Vector2(1.0f, 0.0f); vertices[3].normal = Vector3::UP; vertices[3].tangent = Vector3::RIGHT;

	unsigned short indices[6];
	indices[0] = 0; indices[1] = 2; indices[2] = 1;
	indices[3] = 0; indices[4] = 3; indices[5] = 2;

	shared_ptr<VertexBuffer> vertexBuffer = renderDevice->GetHardwareResourceCreator()->CreateVertexBuffer();
	shared_ptr<IndexBuffer> indexBuffer = renderDevice->GetHardwareResourceCreator()->CreateIndexBuffer();
	vertexBuffer->Initialize(vertices, false, false, VertexFormatType_t::Pos_UV_Normal_Tangent, 4);
	indexBuffer->Initialize(indices, false, false, IndexFormat_t::INT_2_BYTES, 6);

	shared_ptr<SubMesh> subMesh(new SubMesh);
	subMesh->SetVertexBuffer(vertexBuffer);
	subMesh->SetIndexBuffer(indexBuffer);
	subMesh->SetMaterial(Material);

	g_UnitPlaneMesh->AddSubMesh(subMesh);
}

void CreateUnitSphere(const shared_ptr<RenderDevice>& renderDevice, const shared_ptr<Material>& material)
{
    g_UnitSphereMesh.reset(new Mesh);

    // From http://wiki.unity3d.com/index.php/ProceduralPrimitives#C.23_-_Sphere
    const unsigned short longitude = 24;
    const unsigned short latitude = 16;
    const unsigned short numVertices = (longitude + 1) * latitude + 2;

    Vertex_Pos_Normal_t vertices[numVertices];

    for (unsigned short lat = 0; lat < latitude; lat++)
    {
        float a1 = PI * (float)(lat + 1) / (latitude + 1);
        float sin1 = sinf(a1);
        float cos1 = cosf(a1);

        for (unsigned short lon = 0; lon <= longitude; lon++)
        {
            float a2 = PI_TIMES_2 * (float)(lon == longitude ? 0 : lon) / longitude;
            float sin2 = sinf(a2);
            float cos2 = cosf(a2);

            int index = lon + lat * (longitude + 1) + 1;
            vertices[index].position = Vector3(sin1 * cos2, cos1, sin1 * sin2);
            vertices[index].normal = vertices[index].position.Normalized();
        }
    }

    vertices[0].position = Vector3::UP; vertices[0].normal = Vector3::UP;
    vertices[numVertices - 1].position = -Vector3::UP; vertices[numVertices - 1].normal = -Vector3::UP;

    const int numIndices = numVertices * 6;
    unsigned short indices[numIndices];

    int i = 0;
    for (unsigned short lon = 0; lon < longitude; lon++)
    {
        indices[i++] = lon + 2;
        indices[i++] = lon + 1;
        indices[i++] = 0;
    }

    for (unsigned short lat = 0; lat < latitude - 1; lat++)
    {
        for (unsigned short lon = 0; lon < longitude; lon++)
        {
            unsigned short current = lon + lat * (longitude + 1) + 1;
            unsigned short next = current + longitude + 1;

            indices[i++] = current;
            indices[i++] = current + 1;
            indices[i++] = next + 1;

            indices[i++] = current;
            indices[i++] = next + 1;
            indices[i++] = next;
        }
    }

    for (unsigned short lon = 0; lon < longitude; lon++)
    {
        indices[i++] = numVertices - 1;
        indices[i++] = numVertices - (lon + 2) - 1;
        indices[i++] = numVertices - (lon + 1) - 1;
    }

    shared_ptr<VertexBuffer> vertexBuffer = renderDevice->GetHardwareResourceCreator()->CreateVertexBuffer();
    shared_ptr<IndexBuffer> indexBuffer = renderDevice->GetHardwareResourceCreator()->CreateIndexBuffer();
    vertexBuffer->Initialize(vertices, false, false, VertexFormatType_t::Pos_Normal, numVertices);
    indexBuffer->Initialize(indices, false, false, IndexFormat_t::INT_2_BYTES, numIndices);

    shared_ptr<SubMesh> subMesh(new SubMesh);
    subMesh->SetVertexBuffer(vertexBuffer);
    subMesh->SetIndexBuffer(indexBuffer);
    subMesh->SetMaterial(material);

    g_UnitSphereMesh->AddSubMesh(subMesh);
}

}