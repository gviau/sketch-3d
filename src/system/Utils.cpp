#include "system/Utils.h"

#include <sstream>
using namespace std;

namespace Sketch3D {

vector<string> Tokenize(const string& str, char delim) {
    vector<string> tokens;
    stringstream ss(str);
    string item;

    while (getline(ss, item, delim)) {
        if (!item.empty()) {
            tokens.push_back(item);
        }
    }

    return tokens;
}

}