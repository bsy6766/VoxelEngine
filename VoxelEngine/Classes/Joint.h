#ifndef JOINT_H
#define JOINT_H

// cpp
#include <string>
#include <vector>

// glm
#include <glm\glm.hpp>

namespace Voxel
{
	class Joint
	{
	public:
		// Integer id for joint
		int id;
		// String name for joint. not sure if we need this
		std::string name;
		// Children joints of this joint
		std::vector<Joint*> children;

		glm::mat4 animatedTransformMat;

		glm::mat4 localBindTransformMat;
		glm::mat4 inverseBindTransformMat;
	public:
		Joint(const int id, const std::string& name, const glm::mat4& localTransformMat);
		~Joint();

		void addChild(Joint* child);
		void calculateInverseBindTransformMat(const glm::mat4& parentMat);
	};
}

#endif