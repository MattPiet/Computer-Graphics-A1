#include "SkyBox.h"

#include "MemoryMonitor.h"

SkyBox::SkyBox(const char* posXFileName_, const char* negXFileName_, 
	const char* posYFileName_, const char* negYFileName_, 
	const char* posZFileName_, const char* negZFileName_){
	posXFileName = posXFileName_;
	negXFileName = negXFileName_;
	posYFileName = posYFileName_;
	negYFileName = negYFileName_;
	posZFileName = posZFileName_;
	negZFileName = negZFileName_;
}

SkyBox::~SkyBox(){
}

void SkyBox::setSkyBox(std::vector<std::string> textures){
	posXFileName = textures[0].c_str();
	negXFileName = textures[1].c_str();
	posYFileName = textures[2].c_str();
	negYFileName = textures[3].c_str();
	posZFileName = textures[4].c_str();
	negZFileName = textures[5].c_str();
	if (!skyBox->LoadCubemap({
	 posXFileName,
	 negXFileName,
	 posYFileName,
	 negYFileName,
	 posZFileName,
	 negZFileName
		}));
}

bool SkyBox::OnCreate(){
	if (posXFileName == nullptr) {

	}
	shader = new Shader("shaders/skyBoxVert.glsl", "shaders/skyBoxFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Cube Shader failed ... we have a problem\n";
		return false;
	}
	skyBox = new Texture();

	Cube = new Mesh("meshes/Cube.obj");
	Cube->OnCreate();
	if (!skyBox->LoadCubemap({
	 posXFileName,
	 negXFileName,
	 posYFileName,
	 negYFileName,
	 posZFileName,
	 negZFileName
		})) {
		std::cout << "Skybox texture failed ... we have a problem\n";
		return false;
	}
	//textureId
	return true;
}

void SkyBox::OnDestroy(){
	Cube->OnDestroy();
	delete Cube;
	skyBox->OnDestroy();
	delete skyBox;
	shader->OnDestroy();
	delete shader;
	posXFileName = nullptr;
	negXFileName = nullptr;
	posYFileName = nullptr;
	negYFileName = nullptr;
	posZFileName = nullptr;
	negZFileName = nullptr;

}


void SkyBox::Render(){

	glDepthMask(GL_FALSE);
//	std::cout << skyBox->getTextureID() << std::endl;
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox->getTextureID());
	Cube->Render(GL_TRIANGLES);
	glDepthMask(GL_TRUE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
