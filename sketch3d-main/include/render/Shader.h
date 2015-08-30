#ifndef SKETCH_3D_SHADER_H
#define SKETCH_3D_SHADER_H

#include "render/HardwareResource.h"

#include <string>
using namespace std;

namespace Sketch3D {

// Forward class declaration
class VertexFormat;

/**
 * @class Shader
 * Base class for any type of shader implemented by the various rendering API
 */
class SKETCH_3D_API Shader : public HardwareResource {
    public:
        virtual        ~Shader() {}
        bool            InitializeFromFile(const string& filename);
        virtual bool    InitializeFromSource(const string& source) = 0;
};

class SKETCH_3D_API FragmentShader : public Shader {
};

class SKETCH_3D_API VertexShader : public Shader {
    public:
};

}

#endif