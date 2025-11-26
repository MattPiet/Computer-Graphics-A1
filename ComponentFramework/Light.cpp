#include "Light.h"
#include "MemoryMonitor.h"
// uniform buffer object
// for importing objects into glsl

Light::Light(){
	lightPos = Vec3(0.0f, 0.0f, 0.0f);
	kd = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	ks = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

Light::Light(Vec3 lightPos_, Vec4 ks_, Vec4 kd_){
	lightPos = lightPos_; 
	ks = ks_;
	kd = kd_;
}

bool Light::OnCreate()
{
	return true;
}

void Light::UpdateUBO()
{
}
