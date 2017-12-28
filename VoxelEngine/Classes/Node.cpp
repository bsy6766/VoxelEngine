// pch
#include "PreCompiled.h"

#include "Node.h"

Voxel::UI::Node::Node(const std::string & name)
	: TransformNode(name)
{}

Voxel::UI::Node * Voxel::UI::Node::create(const std::string & name)
{
	return new Node(name);
}

void Voxel::UI::Node::render()
{
	if (children.empty())
	{
		return;
	}
	else
	{
		if (visibility)
		{
			for (auto& child : children)
			{
				(child.second)->render();
			}
		}
	}
}
