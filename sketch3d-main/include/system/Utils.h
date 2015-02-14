#ifndef SKETCH_3D_UTILS_H
#define SKETCH_3D_UTILS_H

#include "system/Platform.h"

#include <string>
#include <vector>
using namespace std;

namespace Sketch3D {

vector<string> SKETCH_3D_API Tokenize(const string& str, char delim);

}

#endif