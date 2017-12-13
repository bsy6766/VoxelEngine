#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

// voxel
#include "UIBase.h"

namespace Voxel
{
	namespace UI
	{
		// foward declaration
		class Text;
		class Image;

		/**
		*	@class InputField
		*	@brief InputField is a text that can be modified by clicking the text.
		*/
		class InputField : public TransformNode
		{
		public:
			enum class State
			{
				IDLE = 0,
				HOVERED,
				CLICKED,
				MODIFYING,
			};
		private:
			// Constructor
			InputField() = delete;
			InputField(const std::string& name);
			
			// Destructor
			~InputField();

			// defualt text
			std::string defaultText;

			// State
			State state;

			// Text
			Voxel::UI::Text* text;

			// Cursor.
			Voxel::UI::Image* cursor;

			// Initialize
			bool init(const std::string& defaultText, const int fontId, const std::string& spriteSheetName, const std::string& cursorImageName);
		public:
			static InputField* create(const std::string& name, const std::string& defaultText, const std::string& spriteSheetName, const int fontId, const std::string& cursorImageName);
			
			// default text
			void setDefaultText(const std::string& defaultText);

			// get default text
			std::string getDefaultText() const;

			// override 
			void updateModelMatrix() override;

			// update mouse move
			virtual bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta);

			// update mouse press
			virtual bool updateMousePress(const glm::vec2& mousePosition, const int button);

			// update mouse release 
			virtual bool updateMouseRelease(const glm::vec2& mousePosition, const int button);

			// render input field
			void render() override;
		};
	}
}

#endif