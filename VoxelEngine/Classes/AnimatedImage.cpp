// pch
#include "PreCompiled.h"

#include "AnimatedImage.h"

// voxel
#include "Quad.h"
#include "Utility.h"
#include "ProgramManager.h"
#include "Program.h"

Voxel::UI::AnimatedImage::AnimatedImage(const std::string & name)
	: RenderNode(name)
	, frameSize(0)
	, interval(0.0f)
	, elapsedTime(0.0f)
	, currentFrameIndex(0)
	, currentIndex(0)
	, repeat(false)
	, stopped(false)
	, paused(false)
	, state(State::IDLE)
{}

Voxel::UI::AnimatedImage * Voxel::UI::AnimatedImage::create(const std::string & name, const std::string & spriteSheetName, const std::string& frameName, const int frameSize, const float interval, const bool repeat)
{
	auto newAnimatedImage = new AnimatedImage(name);

	auto& ssm = SpriteSheetManager::getInstance();

	auto ss = ssm.getSpriteSheetByKey(spriteSheetName);

	if (ss)
	{
		if (newAnimatedImage->init(ss, frameName, frameSize, interval, repeat))
		{
			return newAnimatedImage;
		}
	}

	delete newAnimatedImage;
	return nullptr;
}

bool Voxel::UI::AnimatedImage::init(SpriteSheet* ss, const std::string& frameName, const int frameSize, const float interval, const bool repeat)
{
	texture = ss->getTexture();

	if (texture == nullptr)
	{
		return false;
	}

	this->frameSize = frameSize;
	this->interval = interval;
	this->repeat = repeat;

	texture->setLocationOnProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	std::string fileName;
	std::string fileExt;

	Utility::String::fileNameToNameAndExt(frameName, fileName, fileExt);

	auto quadIndices = Quad::indices;

	for (int i = 0; i < frameSize; i++)
	{
		std::string currentFrameName = fileName + "_" + std::to_string(i) + fileExt;

		auto imageEntry = ss->getImageEntry(currentFrameName);

		if (imageEntry)
		{
			auto size = glm::vec2(imageEntry->width, imageEntry->height);
			auto curVertices = Quad::getVertices(size);

			vertices.insert(vertices.end(), curVertices.begin(), curVertices.end());

			auto& uvOrigin = imageEntry->uvOrigin;
			auto& uvEnd = imageEntry->uvEnd;

			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvOrigin.x);
			uvs.push_back(uvEnd.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvOrigin.y);
			uvs.push_back(uvEnd.x);
			uvs.push_back(uvEnd.y);

			for (auto index : quadIndices)
			{
				indices.push_back(index + (4 * i));
			}

			frameSizes.push_back(size);
		}
		else
		{
			return false;
		}
	}

	boundingBox.center = position;
	boundingBox.size = frameSizes.front();

	contentSize = boundingBox.size;

	build(vertices, uvs, indices);

	return true;
}

void Voxel::UI::AnimatedImage::build(const std::vector<float>& vertices, const std::vector<float>& uvs, const std::vector<unsigned int>& indices)
{
	if (vao)
	{
		// Delte array
		glDeleteVertexArrays(1, &vao);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	program = ProgramManager::getInstance().getProgram(ProgramManager::PROGRAM_NAME::UI_TEXTURE_SHADER);

	GLint vertLoc = program->getAttribLocation("vert");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//============= find error here. error count: 14. Only during using sprite sheet
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices.front(), GL_STATIC_DRAW);
	//========================

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &ibo);
}

void Voxel::UI::AnimatedImage::start()
{
	currentFrameIndex = 0;
	elapsedTime = 0.0f;

	paused = false;
	stopped = false;
}

void Voxel::UI::AnimatedImage::pause()
{
	paused = true;
}

void Voxel::UI::AnimatedImage::resume()
{
	paused = false;
}

void Voxel::UI::AnimatedImage::stop()
{
	stopped = true;
}

void Voxel::UI::AnimatedImage::setInterval(const float interval)
{
	this->interval = glm::max(interval, 0.0f);
}

void Voxel::UI::AnimatedImage::update(const float delta)
{
	// If animation is stopped, don't update
	if (stopped) return;

	// If animation is paused, don't update
	if (paused) return;

	// update time
	elapsedTime += delta;

	bool updated = false;

	// loop until elapsed time is less than interval
	while (elapsedTime >= interval)
	{
		// subtract
		elapsedTime -= interval;

		// increment frame index
		currentFrameIndex++;
		// Add index (Each quad takes 6 indices)
		currentIndex += 6;

		// mark as true
		updated = true;

		// check frame index
		if (currentFrameIndex >= frameSize)
		{
			// animation finished
			if (repeat)
			{
				// repeat animation. Reset indices.
				currentFrameIndex = 0;
				currentIndex = 0;
			}
			else
			{
				// This animated image doesn't repeat. Stop
				stopped = true;
				// Revert index
				currentIndex -= 6;

				// Revert flag
				updated = false;
			}
		}
	}

	// Check if frame was changed
	if (updated)
	{
		// Frame changed. Update bounding box size
		boundingBox.size = frameSizes.at(currentFrameIndex);

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_ANIMATED_IMAGE_BOUNDING_BOX
		// If debug, ui bounding box and animated image bounding box is enabled, create debug boudning box line
		createDebugBoundingBoxLine();
#endif
	}
}

bool Voxel::UI::AnimatedImage::updateAnimatedImageMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	// check if progress timer is interactable
	if (isInteractable())
	{
		// check bb
		if (boundingBox.containsPoint(mousePosition))
		{
			if (state == State::IDLE)
			{
				state = State::HOVERED;

				if (onMouseEnter)
				{
					onMouseEnter(this);
				}
			}
			else if (state == State::HOVERED)
			{
				if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
				{
					if (onMouseMove)
					{
						onMouseMove(this);
					}
				}
			}

			return true;
		}
		else
		{
			updateMouseMoveFalse();
		}
	}

	return false;
}

void Voxel::UI::AnimatedImage::updateMouseMoveFalse()
{
	if (state != State::IDLE)
	{
		state = State::IDLE;

		if (onMouseExit)
		{
			onMouseExit(this);
		}
	}
}

bool Voxel::UI::AnimatedImage::updateMouseMove(const glm::vec2 & mousePosition, const glm::vec2 & mouseDelta)
{
	if (visibility)
	{
		// visible
		if (children.empty())
		{
			// Has no children. update self
			return updateAnimatedImageMouseMove(mousePosition, mouseDelta);
		}
		else
		{
			// Has children
			bool childHovered = false;

			// Reverse iterate children because child who has higher global z order gets rendered above other siblings who has lower global z order
			auto rit = children.rbegin();
			for (; rit != children.rend();)
			{
				bool result = (rit->second)->updateMouseMove(mousePosition, mouseDelta);
				if (result)
				{
					// child hovered
					childHovered = true;
					break;
				}

				rit++;
			}

			if (childHovered)
			{
				// There was a child had mouse move event. Iterate remaining children and update

				// Don't forget to increment iterator.
				rit++;

				for (; rit != children.rend(); rit++)
				{
					(rit->second)->updateMouseMoveFalse();
				}

				updateMouseMoveFalse();
			}
			else
			{
				// There was no mouse move event on child
				childHovered = updateAnimatedImageMouseMove(mousePosition, mouseDelta);
			}

			return childHovered;
		}
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::AnimatedImage::updateMousePress(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool pressed = false;

		if (isInteractable())
		{
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					if (state == State::HOVERED)
					{
						state = State::CLICKED;
						pressed = true;

						if (onMousePressed)
						{
							onMousePressed(this, button);
						}
					}
				}
			}
		}

		if (!pressed)
		{
			pressed = Voxel::UI::TransformNode::updateMousePress(mousePosition, button);
		}

		return pressed;
	}
	else
	{
		return false;
	}
}

bool Voxel::UI::AnimatedImage::updateMouseRelease(const glm::vec2 & mousePosition, const int button)
{
	if (visibility)
	{
		bool released = false;

		if (isInteractable())
		{
			if (button == GLFW_MOUSE_BUTTON_1)
			{
				if (boundingBox.containsPoint(mousePosition))
				{
					if (state == State::CLICKED)
					{
						state = State::IDLE;

						released = true;

						if (onMouseReleased)
						{
							onMouseReleased(this, button);
						}
					}
				}
			}
		}

		if (!released)
		{
			released = Voxel::UI::TransformNode::updateMouseRelease(mousePosition, button);
		}

		return released;
	}
	else
	{
		return false;
	}
}

void Voxel::UI::AnimatedImage::renderSelf()
{
	if (texture == nullptr) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();
	texture->enableTexLoc();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(currentIndex * sizeof(GLuint)));
	}

#if V_DEBUG && V_DEBUG_DRAW_UI_BOUNDING_BOX && V_DEBUG_DRAW_ANIMATED_IMAGE_BOUNDING_BOX
	if (bbVao)
	{
		auto lineProgram = ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::LINE_SHADER);
		lineProgram->use(true);
		lineProgram->setUniformMat4("modelMat", modelMat);
		lineProgram->setUniformMat4("viewMat", glm::mat4(1.0f));
		lineProgram->setUniformVec4("lineColor", glm::vec4(1.0f));

		glBindVertexArray(bbVao);
		glDrawArrays(GL_LINES, 0, 8);
	}
#endif
}