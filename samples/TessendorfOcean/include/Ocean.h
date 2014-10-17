#ifndef OCEAN_H
#define OCEAN_H

#include "math/Complex.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

#include "render/Mesh.h"
#include "render/Node.h"

// Forward declarations
namespace Sketch3D {
    class Material;
    class Shader;
}
using namespace Sketch3D;

#include <thread>
#include <vector>
using namespace std;

/**
 * @class Ocean
 * This class simulates ocean waves. The paper that describes the movement is written by Tessendorf, 2001.
 * The actual implementation comes from http://www.keithlantz.net/2011/10/ocean-simulation-part-one-using-the-discrete-fourier-transform/
 */
class Ocean {
    public:
        /**
         * Constructor
         * @param numberOfPoints Specifies the number of points that will compose the Ocean mesh on the X and Z plane
         * @param amplitude The maximum amplitude that the waves will be able to reach
         * @param windStrength A vector describing the wind strength on the XZ plane
         * @param length The length of each cell of the mesh plane
         */
                            Ocean(const int numberOfPoints, const float amplitude, const Vector2& windStrength, const float length);

        /**
         * Destructor
         */
                           ~Ocean();

        /**
         * Evaluates the height and position of each points in the ocean mesh at time t
         */
        void                EvaluateWaves(double t);

        /**
         * Sets the appropriate rendering settings prior to rendering
         */
        void                PrepareForRender();

    private:
        const int           numberOfPoints_;    /**< Number of points on the ocean mesh */
        const float         amplitude_;         /**< The amplitude of the waves */
        Vector2             wind_;              /**< The strength on the wind on the XZ plane */
        float               windLength_;        /**< The length of the wind */
        const float         length_;            /**< Length of each cell of the mesh plane */
        const float         gravity_;           /**< Gravity constant, 9.81 */

        Material*           oceanMaterial_;     /**< The ocean material */
        Mesh                oceanMesh_;         /**< The actual mesh */
        SurfaceTriangles_t  oceanSurface_;      /**< The ocean surface mesh */
        vector<Vector2>     initialPositions_;  /**< Initial positions of the vertices, used to compute the height of the waves */
        Node                oceanNode_;         /**< The node that represents the ocean in the scene tree */
        Shader*             oceanShader_;       /**< The shader that we use to render the ocean */

        // Tessendorf specific variables
        vector<Complex>     hTilde0_;
        vector<Complex>     hTilde0Conjugate_;

        // FFT specific variables
        size_t              log_2_N_;
        size_t*             reversed_;
        Complex**           twiddleFactors_;
        Complex*            c_height_[2];
        Complex*            c_slopex_[2];
        Complex*            c_slopez_[2];
        Complex*            c_dx_[2];
        Complex*            c_dz_[2];
        Complex*            hTildeHeight_;
        Complex*            hTildeSlopex_;
        Complex*            hTildeSlopez_;
        Complex*            hTildeDx_;
        Complex*            hTildeDz_;

        // Tessendord specific functions
        Complex             ComputeHTilde0(int i, int j) const;
        Complex             GaussianRandomVariable() const;
        float               UniformRandomVariable() const;
        float               Phillips(int i, int j) const;
        size_t              Reverse(int i) const;
        Complex             ComputeTwiddleFactor(int x, int dimension) const;
        Complex             ComputeHTilde(double t, int i, int j) const;
        float               Dispersion(int i, int j) const;

        // FFT specific functions
        void                Fft(Complex* input, Complex* output, size_t stride, size_t offset, Complex* c[]);
        void                FftPassHeight();
        void                FftPassSlopex();
        void                FftPassSlopez();
        void                FftPassDx();
        void                FftPassDz();
};

#endif