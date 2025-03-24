#pragma once

#include <glew.h>
#include <vector>
#include <Vector.h>
#include <Triangle.h>
using namespace MATH;

class Mesh {
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator = (const Mesh&) = delete;
	Mesh& operator = (Mesh&&) = delete;

private:
	const char* filename;
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvCoords;
	size_t dateLength;
	GLenum drawmode;

	/// Private helper methods
	void LoadModel(const char* filename);
	void StoreMeshData(GLenum drawmode_);
	GLuint vao, vbo;
public:
	Mesh(const char* filename_);
	~Mesh();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;
	void Render(GLenum drawmode) const;
	
	// Umer messing with Scott’s Mesh class
	// We need the option to create a mesh from a Triangle object
	// default constructor
	Mesh() :
		dateLength{ 0 }
		, drawmode{ 0 }
		, vao{ 0 }
		, vbo{ 0 }
	{}
	////
	// loading the MATHEX triangle vertices
	void LoadTriangle(const MATHEX::Triangle* triangle)
	{
		vertices.clear();
		normals.clear();
		uvCoords.clear();
		vertices.push_back(triangle->getV0());
		vertices.push_back(triangle->getV1());
		vertices.push_back(triangle->getV2());
		// Normals and UVs are empty stubs for now
		for (int i = 0; i < 3; i++) {
			normals.push_back(Vec3());
			uvCoords.push_back(Vec2());
		}
	}
	// changing on create to match with the MATHEX triangle
	// this oncreate is taking in a triangle then calling load triangle so whats above acc comes second
	bool OnCreate(const MATHEX::Triangle* triangle)
	{
		LoadTriangle(triangle);
		StoreMeshData(GL_TRIANGLES);
		return true;
	}

};

