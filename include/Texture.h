#ifndef TEXTURE_H
#define TEXTURE_H 1
#include "Renderer.h"
/*
class Texture {
	private:
		unsigned int m_RendererID;
		std::string m_FilePath;
		unsigned char *m_LocalBuffer;
		int m_Width, m_Height, m_BPP; // bytes per pixel
	public:
		Texture(const std::string& path);
		~Texture();

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		constexpr int GetWidth() const { return m_Width; }
		constexpr int GetHeight() const { return m_Height; }
};
*/

void makeTexture(GLuint *id, const char path[], const GLuint slot);
void bindTexture(const GLuint id, const GLuint slot);
void activateTexture(const GLuint slot);

#endif
