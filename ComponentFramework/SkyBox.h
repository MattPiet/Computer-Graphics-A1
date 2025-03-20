#pragma once
#include <iostream>
#include <glew.h>
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
class SkyBox{
	const char *posXFileName, *negXFileName, *posYFileName, *negYFileName, *posZFileName, *negZFileName;
	Shader* shader;
	Texture* skyBox;
	Mesh* Cube;
	
public:

	SkyBox(const char* posXFileName_, const char* negXFileName_,
		const char* posYFileName_, const char* negYFileName_, 
		const char* posZFileName_, const char* negZFileName_);
	~SkyBox();

	void setSkyBox(std::vector<std::string> textures);

	bool OnCreate();
	void OnDestroy();
	Shader* GetShader() const { return shader; }
	Texture* GetTexture() const{ return skyBox; }
	void Render();
	
};

