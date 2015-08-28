#include "render/Shader.h"

#include "system/Logger.h"

#include <fstream>
using namespace std;

namespace Sketch3D {
bool Shader::InitializeFromFile(const string& filename) {
    ifstream shaderFile(filename);
    if (!shaderFile.is_open()) {
        Logger::GetInstance()->Error("Couldn't open shader file: " + filename);
        return false;
    }

    string source;
    shaderFile.seekg(0, std::ios::end);
    source.resize((size_t) shaderFile.tellg());
    shaderFile.seekg(0, std::ios::beg);

    shaderFile.read(&source[0], source.length());

    if (!InitializeFromSource(source))
    {
        Logger::GetInstance()->Error("Error while creating shader: " + filename);
        return false;
    }

    return true;
}

}