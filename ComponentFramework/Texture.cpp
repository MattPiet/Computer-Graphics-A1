#include "Texture.h"
#include <SDL_image.h>

#include "MemoryMonitor.h"

Texture::Texture() {
	textureID = 0;
}

bool Texture::LoadImage(const char* filename) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	SDL_Surface* textureSurface = IMG_Load(filename);
	if (textureSurface == nullptr) {
		return false;
	}
	int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);
	/// Wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0); /// Unbind the texture
	return true;
}

void Texture::OnDestroy(){
    if (textureID == GL_TEXTURE_CUBE_MAP) {
        glDeleteTextures(GL_TEXTURE_CUBE_MAP, &textureID);
    }
    if(textureID == GL_TEXTURE_2D) {
        glDeleteTextures(GL_TEXTURE_2D, &textureID);
    }
}


Texture::~Texture() {
	glDeleteTextures(1, &textureID);
}

bool Texture::LoadCubemap(const std::vector<std::string>& faces) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (unsigned int i = 0; i < faces.size(); i++) {
        SDL_Surface* surface = IMG_Load(faces[i].c_str());
        if (surface) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, surface->w, surface->h, 0,
                GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
            SDL_FreeSurface(surface);
        }
        else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            return false;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}