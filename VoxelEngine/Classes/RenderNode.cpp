// pch
#include "PreCompiled.h"

#include "RenderNode.h"

Voxel::UI::RenderNode::RenderNode(const std::string & name)
	: TransformNode(name)
	, program(nullptr)
	, vao(0)
	, texture(nullptr)
	, color(1.0f)
{}

Voxel::UI::RenderNode::~RenderNode()
{
	std::cout << "~RenderNode() " + name + "\n";
	if (vao)
	{
		// Delte array
		std::cout << "vao = " << vao << "\n";
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX
	if (bbVao)
	{
		// Delte array
		glDeleteVertexArrays(1, &bbVao);
	}
#endif
}

void Voxel::UI::RenderNode::setColor(const glm::vec3 & color)
{
	this->color = glm::clamp(color, 0.0f, 1.0f);
}

glm::vec3 Voxel::UI::RenderNode::getColor() const
{
	return color;
}

void Voxel::UI::RenderNode::render()
{
	if (children.empty())
	{
		if (visibility)
		{
			renderSelf();
		}
	}
	else
	{
		if (visibility)
		{
			auto children_it = children.begin();
			auto beginZOrder = ((children_it)->first).getGlobalZOrder();

			if (beginZOrder < 0)
			{
				// has negative ordered children
				for (; ((children_it)->first).getGlobalZOrder() < 0; children_it++)
				{
					((children_it)->second)->render();
				}
			}
			// else, doesn't have negative ordered children

			// Render self
			renderSelf();

			// Render positive 
			for (; children_it != children.end(); children_it++)
			{
				((children_it)->second)->render();
			}
		}
	}
}