#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H

// voxel
#include "UIBase.h"

// cpp
#include <functional>

namespace Voxel
{
	// foward declaration
	class InputHandler;

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
				EDITTING,
			};

			enum class Align
			{
				LEFT = 0,
				CENTER,
				RIGHT
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

			// Align
			Align align;

			// input handler
			InputHandler* input;

			// true if text is default text. Need a flag for default text because input field wipes text if text equals to default text on edit start.
			bool textDefaultMode;

			// Text
			Voxel::UI::Text* text;

			// prev text. for cancel event
			std::string prevText;

			// Cursor.
			Voxel::UI::Image* cursor;

			// On input field clicked 
			std::function<void()> onEditStart;
			// Called when user modifies the text.
			std::function<void(const std::string&)> onEdit;
			// Called when user finishes modifying text and confirms
			std::function<void(const std::string&)> onEditFinished;
			// Called when user cancels edit
			std::function<void()> onEditCancelled;

			// Initialize
			bool init(const std::string& defaultText, const int fontId, const std::string& spriteSheetName, const std::string& cursorImageName);

			// update text position
			void updateTextPosition();

			// update cursor
			void updateCursorPosition();

			// modify text
			void modifyText(const std::string& text);
		public:
			static InputField* create(const std::string& name, const std::string& defaultText, const std::string& spriteSheetName, const int fontId, const std::string& cursorImageName);
			
			// default text
			void setDefaultText(const std::string& defaultText);

			// get default text
			std::string getDefaultText() const;

			// Set align
			void setAlign(const Voxel::UI::InputField::Align align);

			// set callback
			void setOnEditStartCallback(const std::function<void()>& func);
			void setOnEditCallback(const std::function<void(const std::string&)>& func);
			void setOnEditFinished(const std::function<void(const std::string&)>& func);
			void setOnEditCancelled(const std::function<void()>& func);
			
			// override 
			void updateModelMatrix() override;
			void updateBoundingBox() override;

			// update
			void update(const float delta) override;
			
			// update mouse move
			bool updateMouseMove(const glm::vec2& mousePosition);
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;

			// update mouse press
			bool updateMousePress(const glm::vec2& mousePosition, const int button) override;

			// update mouse release 
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			// start edit
			void startEdit();

			// Finish edit
			void finishEdit();

			// cancel edit
			void cancelEdit();

			// Add string
			void appendStr(const std::string& str);

			// remove last character
			void removeLastCharacter();

			// set to default text
			void setToDefaultText();

			// render input field
			void render() override;
		};
	}
}

#endif