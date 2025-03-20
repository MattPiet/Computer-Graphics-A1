#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include <Vector.h> /// This is in GameDev
#include <VMath.h>
#include <MMath.h>
#include <QMath.h>
#include <iostream>

using namespace MATH;
class Light {
public:
    Vec4 ks = Vec4(0.3, 0.9, 0.3, 0.0);  // Specular coefficient
    Vec4 kd = Vec4(0.6, 0.6, 0.3, 0.0);  // Diffuse coefficient
    Vec3 lightPos;                       // Light position
    Vec4 lightColor = Vec4(1.0, 1.0, 1.0, 1.0);  // Default white light
    float intensity = 1.0f;

   
    // Constructors
    Light();
    Light(Vec3 lightPos_, Vec4 ks_, Vec4 kd_);

    // Initialize OpenGL resources
    bool OnCreate();

    // Update the UBO with current light properties
    void UpdateUBO();
};

#endif