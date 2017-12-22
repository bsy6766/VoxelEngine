#ifndef INPUT_FIELD_2_H
#define INPUT_FIELD_2_H

// voxel
#include "Text.h"

#include <functional>

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class InputField
		*	@brief A text that can be modified.
		*
		*	Inherits Text class. Simply adds cursor and text modification features.
		*	Because Text class uses own font texture, InputField needs 2 draw call at max for text cursor.
		*	If cursor is invisible, only 1 draw call occurs just like Text.
		*/
		class InputField : public Text
		{
		private:
			InputField() = delete;
			InputField(const std::string& name);
			~InputField();

			// cursor 
			GLuint cursorVao;
			glm::mat4 cursorModelMat;
			Texture2D* cursorTexture;
			Program* cursorProgram;
			const float cursorBlinkSpeed;
			float cursorBlinkElapsedTime;
			bool cursorVisible;

			// text limit. -1 meahs infinite
			int textMaxLength;

			// default text
			std::string defaultText;
			bool defaultTextMode;
			bool prevDefaultTextMode;

			// prev text. Restores text to prev text if cancels.
			std::string prevText;

			// edit flag
			bool editting;

			// On input field clicked 
			std::function<void(Voxel::UI::InputField*)> onEditStart;
			// Called when user modifies the text.
			std::function<void(Voxel::UI::InputField*, const std::string)> onEdit;
			// Called when user finishes modifying text and confirms
			std::function<void(Voxel::UI::InputField*, const std::string)> onEditFinished;
			// Called when user cancels edit
			std::function<void(Voxel::UI::InputField*)> onEditCancelled;

			// Initialize
			bool init(const std::string& defaultText, const int fontId, const std::string& spriteSheetName, const std::string& cursorImageName, const Voxel::UI::Text::ALIGN align, const int textMaxLength, const unsigned int lineBreakWidth);

			// update input field mouse move
			bool updateInputFieldMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta);

			// modify text
			void modifyText(const std::string& text);

			// update cursor position
			void updateCursorModelMatrix();
		public:
			// Creates input field.
			static InputField* create(const std::string& name, const std::string& defaultText, const std::string& spriteSheetName, const int fontId, const std::string& cursorImageName, const Voxel::UI::Text::ALIGN align = Voxel::UI::Text::ALIGN::LEFT, const int textMaxLength = -1, const unsigned int lineBreakWidth = 0);

			// default text
			void setDefaultText(const std::string& defaultText);

			// get default text
			std::string getDefaultText() const;

			// Set text to default text
			void setToDefaultText();
			
			// start edit
			void startEdit();

			// Finish edit. Either pressed mouse out of text or pressed enter.
			void finishEdit();
			
			// cancel edit. Pressed escape key.
			void cancelEdit();

			// Add string
			void appendStr(const std::string& str);

			// remove last character
			void removeLastCharacter();

			// remove last word
			void removeLastWord();

			// Set on edit start callback. This is called when mouse is pressed on text and start to edit.
			void setOnEditStartCallback(const std::function<void(Voxel::UI::InputField*)>& func);

			// Set on edit callback. This is called when text is editted.
			void setOnEditCallback(const std::function<void(Voxel::UI::InputField*, const std::string)>& func);

			// Set on edit finished callback. This is called when editting is done.
			void setOnEditFinishedCallback(const std::function<void(Voxel::UI::InputField*, const std::string)>& func);

			// Set on edit cancelled. This is called when editting is cancelled.
			void setOnEditCancelledCallback(const std::function<void(Voxel::UI::InputField*)>& func);

			// overrides
			void update(const float delta) override;
			bool updateMouseMove(const glm::vec2& mousePosition, const glm::vec2& mouseDelta) override;
			bool updateMousePress(const glm::vec2& mousePosition, const int button) override;
			bool updateMouseRelease(const glm::vec2& mousePosition, const int button) override;

			// render text and cursor
			void renderSelf() override;
		};
	}
}

#endif