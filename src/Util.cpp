#include "Util.h"
#include <iostream>
#include <fstream>

using std::ifstream;

bool fileExists (const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}