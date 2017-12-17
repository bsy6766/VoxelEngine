#include "BinPacker.h"

Voxel::Bin::ItemNode::ItemNode()
	: left(nullptr)
	, right(nullptr)
	, area(Rect())
{}

Voxel::Bin::ItemNode::~ItemNode()
{
	if (left)
	{
		delete left;
	}

	if (right)
	{
		delete right;
	}
}

bool Voxel::Bin::ItemNode::isLeaf() const
{
	return (left == nullptr) && (right == nullptr);
}

float Voxel::Bin::ItemNode::getAreaWidth() const
{
	return this->area.size.x;
}

float Voxel::Bin::ItemNode::getAreaHeight() const
{
	return this->area.size.y;
}




Voxel::Bin::BinPacker::BinPacker()
	: root(nullptr)
{}

Voxel::Bin::BinPacker::~BinPacker()
{
	if (root)
	{
		delete root;
	}
}

void Voxel::Bin::BinPacker::init(const glm::vec2 & boundary)
{
	root = createItemNode(glm::vec2(0), boundary);
}

Voxel::Bin::ItemNode * Voxel::Bin::BinPacker::createItemNode(const glm::vec2& origin, const glm::vec2& size)
{
	ItemNode* newNode = new Voxel::Bin::ItemNode();
	newNode->area.origin = origin;
	newNode->area.size = size;
	return newNode;
}

Voxel::Bin::ItemNode * Voxel::Bin::BinPacker::insert(const glm::vec2 & size)
{
	return insert(root, size);
}

Voxel::Bin::ItemNode * Voxel::Bin::BinPacker::insert(ItemNode * itemNode, const glm::vec2 & size)
{
	if (size.x == 0 || size.y == 0)
	{
		// invalid size
		//return false;
		return nullptr;
	}

	if (itemNode->isLeaf() == false)
	{
		// need both left and right
		if (itemNode->left && itemNode->right)
		{
			// try to insert on left
			auto insertedNode = insert(itemNode->left, size);

			if (insertedNode)
			{
				// done.
				return insertedNode;
			}
			else
			{
				// Failed. try to insert on right
				return insert(itemNode->right, size);
			}
		}
		else
		{
			// either left or right is null
			//return false;
			return nullptr;
		}
	}
	else
	{
		// It's leaf

		// Check if this node already has rect assigned
		if ((itemNode->rect.size == glm::vec2(0.0f) && itemNode->rect.origin == glm::vec2(0.0f)) == false)
		{
			// This node is already allocated.
			//return false;
			return nullptr;
		}
		// Else, it's empty

		// Check if the new rect fits to this node
		const glm::vec2 origin = itemNode->area.origin;
		Rect targetArea = Rect();
		targetArea.origin = origin;
		targetArea.size = size;

		if (itemNode->area.containsRect(targetArea) == false)
		{
			// target rect is too big. failed
			//return false;
			return nullptr;
		}
		// else, can fit

		// Check if this image perfectly fits to area
		if (itemNode->area.size == size)
		{
			itemNode->rect = targetArea;
			return itemNode;
		}
		// Else, doesn't fit perfectly

		// then, split area
		itemNode->left = createItemNode(glm::vec2(0), glm::vec2(0));
		itemNode->right = createItemNode(glm::vec2(0), glm::vec2(0));

		// Check the size of new rect and see which way to split
		const glm::vec2 dSize = itemNode->area.size - size;

		if (dSize.x > dSize.y)
		{
			// The rectangle's width is larger or equal than height, let's call this wide shape (I know this handles square but just being simple)
			// In this case, we split(cut) child area vertically 
			/*
                                area rectagnle					  area rectangle
                                *----------------*					*-----*----------*
                                |     | dh       |					|     |          |
                                |     |     dw   |					|     |          |
                                *-----*----------|		Split		|     |          |
                                |XXXXX|          |		---->		|     |          |
			new rectangle	->	|XXXXX|          |					|     |          |
			(filled with X)		|XXXXX|          |					|     |          |
                                *-----*----------*					*-----*----------*
			*/

			// left origin equals to area's origin
			const glm::vec2 leftOrigin = origin;
			// left size. width equasl to new rect's width and height is same as area
			const glm::vec2 leftSize = glm::vec2(size.x, itemNode->area.size.y);
			// set area
			itemNode->left->area.origin = leftOrigin;
			itemNode->left->area.size = leftSize;

			// Right origin. x Start from area's origin and left size's width, which includes the pad
			float rightX = origin.x + leftSize.x;
			glm::vec2 rightOrigin = glm::vec2(rightX, origin.y);
			// right width is difference between area awidht and leftsize with
			glm::vec2 rightSize = glm::vec2(itemNode->area.size.x - leftSize.x, itemNode->area.size.y);
			// Set area
			itemNode->right->area.origin = rightOrigin;
			itemNode->right->area.size = rightSize;
		}
		else
		{
			// The rectangle's height is larger than width, let's call this long shape rectangle.
			// In this case, we split(cut) child area horizontally 

			/*
                                area rectagnle					  area rectangle
                                *----------------*					*----------------*
                                |         |      |					|                |
                                |      dh |      |					|                |
                                |         |      |		Split		|                |
                                *---------*------|		---->		*----------------*
			new rectangle	->	|XXXXXXXXX|  dw  |					|                |
			(filled with X)		|XXXXXXXXX|      |					|                |
                                *---------*------*					*----------------*
			*/

			// Left origin equals to area's origin
			glm::vec2 leftOrigin = origin;
			glm::vec2 leftSize = glm::vec2(itemNode->area.size.x, size.y);
			itemNode->left->area.origin = leftOrigin;
			itemNode->left->area.size = leftSize;

			float rightY = size.y + origin.y;
			glm::vec2 rightOrigin = glm::vec2(origin.x, rightY);
			float rightHeight = itemNode->area.size.y - size.y;
			glm::vec2 rightSize = glm::vec2(itemNode->area.size.x, rightHeight);
			itemNode->right->area.origin = rightOrigin;
			itemNode->right->area.size = rightSize;
		}

		return insert(itemNode->left, size);
	}
}