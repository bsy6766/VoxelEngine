#include "Joint.h"

using namespace Voxel;

Voxel::Joint::Joint(const int id, const std::string & name, const glm::mat4 & localTransformMat)
	: id(id)
	, name(name)
	, localBindTransformMat(localTransformMat)
{}

Joint::~Joint()
{
	for (auto joint : children)
	{
		if (joint)
		{
			delete joint;
		}
	}

	children.clear();
}

void Voxel::Joint::addChild(Joint * child)
{
	children.push_back(child);
}

void Voxel::Joint::calculateInverseBindTransformMat(const glm::mat4 & parentMat)
{
	glm::mat4 bindTransform = parentMat * this->localBindTransformMat;
	this->inverseBindTransformMat = glm::inverse(bindTransform);

	for (auto child : children)
	{
		child->calculateInverseBindTransformMat(bindTransform);
	}
}


