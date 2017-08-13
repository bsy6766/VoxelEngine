#include "Chunk.h"

using namespace Voxel;

Chunk::Chunk()
	: x(0)
	, y(0)
	, z(0)
{}

Chunk* Chunk::create(const int x, const int y, const int z)
{
	Chunk* newChunk = new Chunk();
	if (newChunk->init(x, y, z))
	{
		return newChunk;
	}
	else
	{
		delete newChunk;
		return nullptr;
	}
}

bool Chunk::init(const int x, const int y, const int z)
{
	this->x = x;
	this->y = y;
	this->z = z;

	return true;
}