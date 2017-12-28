#ifndef NODE_H
#define NODE_H

// voxel
#include "TransformNode.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Node
		*	@brief An empty node that derives TransformNode. 
		*
		*	Use case: Can be used as invisible ui in ui hierarchy.
		*/
		class Node : public TransformNode
		{
		private:
			// Constructor
			Node() = delete;
			Node(const std::string& name);
		public:
			// Destructor
			~Node() = default;

			// create
			static Node* create(const std::string& name);

			// render override
			void render() override;
		};
	}
}

#endif