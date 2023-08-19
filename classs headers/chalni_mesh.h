#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\shader_class_m.h>

#include <iostream>
#include<vector>
using namespace std;
// We store each of the required vertex attributes in a struct called Vertex.
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
};

//We store the id of the texture and its type e.g. a diffuse or specular texture
//struct Texture {
//	unsigned int id;
//	string type;
//	string path; // store path of texture to compare with other textures
//};

class Mesh {
public:
	// mesh data
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	//vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->indices = indices;
		//this->textures = textures;
		setupMesh();
	}
	void Draw(Shader& shader)
	{

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}

private:
	// render data
	unsigned int VAO, VBO, EBO;
	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
			&vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() *
			sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(void*)offsetof(Vertex, Normal));
		// vertex colors
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(void*)offsetof(Vertex, Color));

		glBindVertexArray(0);

	}
};
#endif
#pragma once
