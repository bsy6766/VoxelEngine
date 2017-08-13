#ifndef CHUNK_H
#define CHUNK_H

namespace Voxel
{
	/**
	*	@class Chunk
	*	@brief A chunk of data that contains 16 x 16 x 16 blocks.
	*/
	class Chunk
	{
	private:
		Chunk();

		// Chunk position in the world
		int x;
		int y;
		int z;

		bool init(int x, int y, int z);
	public:
		static Chunk* create(const int x, const int y, const int z);
	};

}
#endif