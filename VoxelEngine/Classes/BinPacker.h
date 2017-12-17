#ifndef BIN_PACKER_H
#define BIN_PACKER_H

// glm
#include <glm\glm.hpp>

// cpp 

namespace Voxel
{
	namespace Bin
	{
		struct Rect
		{
		public:
			glm::vec2 origin;
			glm::vec2 size;

			Rect() : origin(0), size(0) {}
			Rect(const glm::vec2& origin, const glm::vec2& size) : origin(origin), size(size) {}

			float Rect::getMaxX() const
			{
				return origin.x + size.x;
			}

			float Rect::getMidX() const
			{
				return origin.x + (size.x * 0.5f);
			}

			float Rect::getMinX() const
			{
				return origin.x;
			}

			float Rect::getMaxY() const
			{
				return origin.y + size.y;
			}

			float Rect::getMidY() const
			{
				return origin.y + (size.y * 0.5f);
			}

			float Rect::getMinY() const
			{
				return origin.y;
			}

			bool containsRect(const Rect& other)
			{
				if (other.getMaxX() <= getMaxX() &&
					other.getMinX() >= getMinX() &&
					other.getMaxY() <= getMaxY() &&
					other.getMinY() >= getMinY())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		};
		/**
		*	@class Item
		*	@brief Item that goes in to bin in bin packing algorithm
		*/
		class ItemNode
		{
		public:
			// Constructor
			ItemNode();

			// Destructor
			~ItemNode();

			// Item on the left
			ItemNode * left;
			// Item on the right
			ItemNode* right;

			// Area of this node
			Rect area;
			// Allocated area of this node
			Rect rect;

			// Check if this item node has no children (left, right)
			bool isLeaf() const;

			// Get area width
			float getAreaWidth() const;

			// Get area height
			float getAreaHeight() const;
		};
		/**
		*	@class Pakcer
		*	@brief Runs Bin packing algorithm.
		*
		*	This was created only to generate font texture.
		*/
		class BinPacker
		{
		private:
			// Root node
			ItemNode * root;

			ItemNode* createItemNode(const glm::vec2& origin, const glm::vec2& size);

			// insert item node
			ItemNode* insert(ItemNode* itemNode, const glm::vec2& size);
		public:
			BinPacker();
			~BinPacker();

			void init(const glm::vec2& boundary);
			ItemNode* insert(const glm::vec2& size);
		};
	}
}

#endif