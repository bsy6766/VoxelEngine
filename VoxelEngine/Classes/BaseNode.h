#ifndef UI_H
#define UI_H

// Config
#include "Config.h"

// glm
#include <glm/glm.hpp>

// gl
#include <GL\glew.h>

// cpp
#include <string>
#include <map>
#include <list>
#include <functional>

// Voxel
#include "ZOrder.h"
#include "Shape.h"

namespace Voxel
{
	// Forward declarations
	class Texture2D;
	class Font;
	class Program;

	namespace UI
	{
		// Forward declaration
		class BaseNode;
		class Action;

		typedef std::map<ZOrder, TransformNode*, ZOrderComp> Children;

		/**
		*	@class BaseNode
		*	@brief Base class of all UI component
		*	
		*	Node is simple base class that has id number and name. 
		*	Node itself does nothing and can't be created as instance.
		*/
		class BaseNode
		{
		private:
			static unsigned int idCounter;
		protected:
			// Constructor
			BaseNode() = delete;
			BaseNode(const std::string& name);

			// name
			std::string name;

			// id
			unsigned int id;
		public:
			// Destructor
			~BaseNode() = default;

			/**
			*	Get number id of ui
			*/
			unsigned int getID() const;

			/**
			*	Get name of ui
			*/
			std::string getName() const;
		};
	}
}

#endif