#ifndef NINE_PATCH_IMAGE_H
#define NINE_PATCH_IMAGE_H

// voxel
#include "UIBase.h"
#include "SpriteSheet.h"

// cpp
#include <array>

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class NinePatchImage
		*	@brief Divide image in to 9 section and stretches.
		*/
		class NinePatchImage : public RenderNode
		{
		private:
			// Constructor
			NinePatchImage() = delete;
			NinePatchImage(const std::string& name);

			// init
			bool init(SpriteSheet* ss, const std::string& textureName, const float leftPadding, const float rightPadding, const float topPadding, const float bottomPadding, const glm::vec2& bodySize);

			// build quad section
			void buildQuadSection(const glm::vec2& min, const glm::vec2& max, const int offset, std::array<float, 48>& vertices);
			void buildUVs(const glm::vec2& min, const glm::vec2& max, const int offset, std::array<float, 32>& vertices);
			void buildIndices(std::array<unsigned int, 54>& indices, const int offset, const int shift);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad. Single image has 12 vertices.
			*	@param uvs Texture coordinates of image quad. Single image has 8 uv coordinates
			*	@param indices Indices of image quad. Single image has 6 indices
			*/
			virtual void build(const std::array<float, 48>& vertices, const std::array<float, 32>& uvs, const std::array<unsigned int, 54>& indices);
		public:
			// Destructor
			~NinePatchImage() = default;

			/**
			*	Create nine patch image
			*/
			static NinePatchImage* create(const std::string& name, const std::string& spriteSheetName, const std::string& imageFileName, const float leftPadding, const float rightPadding, const float topPadding, const float bottomPadding, const glm::vec2& bodySize);

			/**
			*	Render self
			*/
			void renderSelf() override;
		};
	}
}

#endif