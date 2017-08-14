#include "Utility.h"

using namespace Voxel;

size_t Utility::Random::seedNumber;
std::string Utility::Random::seedString;
std::mt19937 Utility::Random::generator;
bool Utility::Random::initialized = false;