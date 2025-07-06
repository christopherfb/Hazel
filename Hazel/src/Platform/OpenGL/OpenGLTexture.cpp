#include "hzpch.h"
#include "OpenGLTexture.h"
#include "stb_image.h"

#define USING_OPENGL_V4_5_OR_HIGHER 

namespace Hazel {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		HZ_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;
		InitCommonOpenGLSettings();
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		HZ_PROFILE_FUNCTION();

	
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);

		stbi_uc* data = nullptr;
		{
			HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&) ");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		HZ_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0;
		GLenum dataFormat = 0;

		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		HZ_CORE_ASSERT(internalFormat, "Internal Format not supported!");
		HZ_CORE_ASSERT(dataFormat, "Data Format not supported!");
		
		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;


		InitCommonOpenGLSettings();

		// not sure if I need this anymore (video removed SubImage2D code (below) and this was
		// what I used instead of that to make things work earlier.
		glTexImage2D(GL_TEXTURE_2D, 0, m_DataFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// -- end of OpenGL < 4.2+

		stbi_image_free(data);		// deallocate the mem (we've uploaded the data to the gpu)
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		HZ_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_RendererID);
	}



	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		HZ_PROFILE_FUNCTION();
		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		HZ_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Data must be entire texture!");
		// not sure if I need this anymore (video removed SubImage2D code (below) and this was
		// what I used instead of that to make things work earlier.
		glTexImage2D(GL_TEXTURE_2D, 0, m_DataFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		HZ_PROFILE_FUNCTION(); 
		
		// use for OpenGL 4.5+ 
#ifdef USING_OPENGL_V4_5_OR_HIGHER
		glBindTextureUnit(slot, m_RendererID);
#else		 	
		// use for < OpenGL 4.5
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
#endif

	}


	void OpenGLTexture2D::InitCommonOpenGLSettings()
	{
		// Video 42: Textures didn't work for me - instead of displaying a texture on the square, it was black.
		// There was a comment stating:
		//  glTexStorage2D() is only for 4.2 + , glTextureStorage2D() is only for 4.5 +
		// 
		// use for OpenGL < version 4.5
		// -- old computers (as shown in video comment section - with a modification from chatGPT to solve blur issue)
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		// The result was a blurry image - chatGPT recommended replacing m_RendererID with GL_TEXTURE_2D, which worked:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

}
