#include <glad/gl.h>
#include "assets/texture.hpp"
#include "stbImage/stb_image.h"
#include "core/log.hpp"


Texture::Texture(const char* filepath,uint32_t wrapS, uint32_t wrapT, uint32_t minFilter, uint32_t magFilter, bool HDR)
{
	stbi_set_flip_vertically_on_load(true);


	int width, height, channels;
	unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);

	if (channels == 1) spdlog::error("1 channnel: {}", filepath);
	if (data) init(width, height, channels, data, HDR, wrapS , wrapT , minFilter , magFilter);
	else spdlog::error("Failed to load texture from filepath: {}", filepath);
	stbi_image_free(data);
}

Texture::Texture(const TextureDescription& desc)
{
	init(desc.width, desc.height, desc.channels, desc.data, desc.isHDR, desc.wrapS, desc.wrapT, desc.minFilter, desc.magFilter);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ID);
}

void Texture::edit(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, unsigned char* data)
{
	if (data) {
		if (m_channels == 3) glTextureSubImage2D(m_ID, 0, xOffset, yOffset, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (m_channels == 4) glTextureSubImage2D(m_ID, 0, xOffset, yOffset, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
}



void Texture::init(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, bool isHDR, uint32_t wrapS, uint32_t wrapT, uint32_t minFilter, uint32_t magFilter)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapS);
	glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapT);

	glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilter);
	glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, magFilter);

	int32_t mipCount = 1 + floor(log2(std::max(width, height)));

	if (channels == 0) { // HACK for depth, maybe good to rpelace this
		glTextureStorage2D(m_ID, mipCount, GL_DEPTH_COMPONENT32F, width, height);
		if (data) glTextureSubImage2D(m_ID, 0, 0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, data);
	}
	else if (channels == 3) {
		if (isHDR) {
			glTextureStorage2D(m_ID, mipCount, GL_RGBA16F, width, height);
			if (data) glTextureSubImage2D(m_ID, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
		}
		else {
			glTextureStorage2D(m_ID, mipCount, GL_RGBA8, width, height);
			if (data) glTextureSubImage2D(m_ID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
	}
	else if (channels == 4) {
		if (isHDR) {
			glTextureStorage2D(m_ID, mipCount, GL_RGBA16F, width, height);
			if (data) glTextureSubImage2D(m_ID, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, data);
		}
		else {
			glTextureStorage2D(m_ID, mipCount, GL_RGBA8, width, height);
			if (data) glTextureSubImage2D(m_ID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}
	else return;

	glGenerateTextureMipmap(m_ID);

	m_width = width;
	m_height = height;
	m_channels = channels;
	m_isHDR = isHDR;
}

