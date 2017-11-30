#ifndef IMAGE_H
#define IMAGE_H

#include "UIBase.h"

namespace Voxel
{
	namespace UI
	{
		/**
		*	@class Image
		*	@brief Rectangular png ui image that renders on screen space
		*
		*	Simple image that renders in screen space.
		*	Image will be static and won't be changed nor animated
		*	Uses 1 vao and 1 draw call
		*/
		class Image : public RenderNode
		{
		private:
			Image() = delete;

			/**
			*	Constructor
			*	@param name Name of ui
			*	@param imageName Name of image
			*/
			Image(const std::string& name);

			/**
			*	Initialize image
			*	@param textureName Name of image texture file
			*	@return true if successfully creates ui image. Else, false.
			*/
			bool init(const std::string& textureName);

			/**
			*	Initialize image
			*	@param ss SpriteSheet instance.
			*	@param textureName Name of image texture file
			*	@return true if successfully creates ui image. Else, false.
			*/
			bool initFromSpriteSheet(SpriteSheet* ss, const std::string& textureName);

			/**
			*	Build image.
			*	Initialize vao.
			*	@param vertices Vertices of image quad. Single image has 12 vertices.
			*	@param uvs Texture coordinates of image quad. Single image has 8 uv coordinates
			*	@param indices Indices of image quad. Single image has 6 indices
			*/
			virtual void build(const std::array<float, 12>& vertices, const std::array<float, 8>& uvs, const std::array<unsigned int, 6>& indices);
		public:
			// Destructor.
			~Image();

			/**
			*	Creates image with single image file.
			*	If texture exists with same name, uses existing texture. Else, creates new texture.
			*	@param imageFileName Name of image file.
			*	@return Image instance if successfully loads image and creates ui. Else, nullptr if anything fails.
			*/
			static Image* create(const std::string& name, std::string& imageFileName);

			/**
			*	Creates image from sprite sheet.
			*	@param spriteSheetName Name of image file.
			*	@param imageFileName Name of image file.
			*	@return Image instance if successfully loads image and creates ui. Else, nullptr if anything fails.
			*/
			static Image* createFromSpriteSheet(const std::string& name, const std::string& spriteSheetName, const std::string& imageFileName);

			/**
			*	Render self
			*/
			void renderSelf() override;
		};
	}
}

#endif // ! IMAGE_H
