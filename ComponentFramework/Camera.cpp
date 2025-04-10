
#include "Camera.h"
#include <SDL.h>
#include <MMath.h>

Camera::Camera() {
	projection = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 10000.0f);
	orientation = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f));
	position = Vec3(0.0f, 0.0f, -25.0f);
	view = MMath::toMatrix4(orientation) * MMath::translate(position);

}

void Camera::dontTrackY(){
	trackball.Trackingx = false;
	trackball.Trackingz = false;
}

void Camera::setSkyBox(std::vector<std::string> skybox_textures){
	skybox->setSkyBox(skybox_textures);
}

void Camera::SetView(const Quaternion& orientation_, const Vec3& position_) {
	orientation = orientation_;
	position = position_;

}

void Camera::OnDestroy() {
	if (skybox) {
		skybox->OnDestroy();
		delete skybox;
	}
}

Camera::~Camera() {}

bool Camera::OnCreate() {
	skybox = new SkyBox("textures/StarSkyboxPosx.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_X 
		"textures/StarSkyboxNegx.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
		"textures/StarSkyboxPosy.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
		"textures/StarSkyboxNegY.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		"textures/StarSkyboxPosz.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z// this is technically the back side but because insides are 
		// flipped when looking inside a skybox  i need to make it the front side the same applies with the back jpg
		"textures/StarSkyboxnegz.png"); 

	if (skybox->OnCreate() == false) {
		return false;
	}

	return true;

}

void Camera::HandelEvents(const SDL_Event& sdlEvent) {
	trackball.HandleEvents(sdlEvent);
	orientation = trackball.getQuat();

}

/// Our PASS tutor didn't like the name Render since it can't render itself. 
/// I hope this meets with his approval
void Camera::RenderSkyBox() const {
	if (skybox == nullptr) return;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glUseProgram(skybox->GetShader()->GetProgram());
	glUniformMatrix4fv(skybox->GetShader()->GetUniformID("projectionMatrix"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(skybox->GetShader()->GetUniformID("viewMatrix"), 1, GL_FALSE, MMath::toMatrix4(orientation));
	/// Here I has turned on the shader and set the matricies. The shader will remain in this state
	/// until I turn off the shader. In Skybox::Render, I will bind the textures, because that is where
	/// they are, then draw the cube.  
	skybox->Render();
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
	
}
