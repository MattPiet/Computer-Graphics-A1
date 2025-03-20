#ifndef TEXTURE_H
#define TEXTURE_H
#include <glew.h>
#include <vector>
#include <string>
#include <iostream>
class Texture {
	GLuint textureID;
public:
	Texture();
	~Texture();
	bool LoadCubemap(const std::vector<std::string>& faces);
	bool LoadImage(const char* filename);
	inline GLuint getTextureID() const { return textureID; }
	void OnDestroy();

};
#endif
