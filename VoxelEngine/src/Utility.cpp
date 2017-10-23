#include "Utility.h"

using namespace Voxel;

size_t Utility::Random::seedNumber;
std::string Utility::Random::seedString;

#ifdef _WIN64
std::mt19937_64 Utility::Random::generator;
#else
std::mt19937 Utility::Random::generator;
#endif

bool Utility::Random::initialized = false;