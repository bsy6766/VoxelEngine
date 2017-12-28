// pch
#include "PreCompiled.h"

#include "BaseNode.h"

unsigned int Voxel::UI::BaseNode::idCounter = 0;

Voxel::UI::BaseNode::BaseNode(const std::string & name)
	: name(name)
	, id(++BaseNode::idCounter)
{}

unsigned int Voxel::UI::BaseNode::getID() const
{
	return id;
}

std::string Voxel::UI::BaseNode::getName() const
{
	return name;
}