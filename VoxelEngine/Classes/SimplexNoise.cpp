// pch
#include "PreCompiled.h"

#include "SimplexNoise.h"

using namespace Voxel;
using namespace Voxel::Noise;

std::unique_ptr<SimplexNoise> Manager::worldNoise = nullptr;
std::unique_ptr<SimplexNoise> Manager::temperatureNoise = nullptr;
std::unique_ptr<SimplexNoise> Manager::moistureNoise = nullptr;
std::unique_ptr<SimplexNoise> Manager::colorNoise = nullptr;

const float SimplexNoise::F2 = 0.366025403f;
const float SimplexNoise::G2 = 0.211324865f;

const int SimplexNoise::grad3[12][3] =
{ { 1,1,0 },{ -1,1,0 },{ 1,-1,0 },{ -1,-1,0 },
{ 1,0,1 },{ -1,0,1 },{ 1,0,-1 },{ -1,0,-1 },
{ 0,1,1 },{ 0,-1,1 },{ 0,1,-1 },{ 0,-1,-1 } };

Voxel::Noise::SimplexNoise::SimplexNoise(const std::string& seed)
{
	init(seed);
}

float Voxel::Noise::SimplexNoise::noise(const glm::vec2 & v)
{
	// Reference: http://webstaff.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf

	// Noise contributions from the three corners
	float n0, n1, n2;

	// Skew the input space to determine which simplex cell we're in
	float s = (v.x + v.y) * F2; // Hairy factor for 2D
	int i = static_cast<int>(fastFloor(v.x + s));
	int j = static_cast<int>(fastFloor(v.y + s));

	float t = static_cast<float>(i + j) * G2;
	float X0 = i - t;	// Unskew the cell origin back to (x,y) space
	float Y0 = j - t;
	float x0 = v.x - X0; // The x,y distances from the cell origin
	float y0 = v.y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0)
	{
		// lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1;
		j1 = 0;
	}
	else
	{
		// upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1 = 0;
		j1 = 1;
	}

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii + perm[jj]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

	// Calculate the contribution from the three corners
	float t0 = 0.5f - (x0 * x0) - (y0 * y0);
	if (t0 < 0.0f)
	{
		n0 = 0.0f;
	}
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
	}

	float t1 = 0.5f - (x1 * x1) - (y1 * y1);
	if (t1 < 0)
	{
		n1 = 0.0;
	}
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}

	float t2 = 0.5f - (x2 * x2) - (y2 * y2);
	if (t2 < 0)
	{
		n2 = 0.0;
	}
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0f * (n0 + n1 + n2);
}

void Voxel::Noise::SimplexNoise::init(const std::string & seed)
{
	rand.setSeed(seed);

	randomize();
}

void Voxel::Noise::SimplexNoise::randomize()
{
	for (int i = 0; i < 256; i++)
	{
		perm[i] = perm[i + 256] = rand.randRangeInt(1, 255);
	}
}

float Voxel::Noise::SimplexNoise::fastFloor(float value)
{
	int result = value > 0 ? static_cast<int>(value) : static_cast<int>(value) - 1;
	return static_cast<float>(result);
}

// Dot product
float Voxel::Noise::SimplexNoise::dot(const int* grad, const float x, const float y)
{
	return grad[0] * x + grad[1] * y;
}



void Voxel::Noise::Manager::init(const std::string & seed)
{
	worldNoise = std::unique_ptr<SimplexNoise>(new SimplexNoise(seed));
	temperatureNoise = std::unique_ptr<SimplexNoise>(new SimplexNoise(seed + "TEMP"));
	moistureNoise = std::unique_ptr<SimplexNoise>(new SimplexNoise(seed + "MOIS"));
	colorNoise = std::unique_ptr<SimplexNoise>(new SimplexNoise(seed + "COLOR"));
}

SimplexNoise * Voxel::Noise::Manager::getWorldNoise()
{
	return worldNoise.get();
}

SimplexNoise * Voxel::Noise::Manager::getTemperatureNoise()
{
	return temperatureNoise.get();
}

SimplexNoise * Voxel::Noise::Manager::getMoistureNoise()
{
	return moistureNoise.get();
}

SimplexNoise * Voxel::Noise::Manager::getColorNoise()
{
	return colorNoise.get();
}
