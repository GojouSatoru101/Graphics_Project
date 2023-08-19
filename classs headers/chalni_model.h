#pragma once
#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <C:\Users\DELL\source\repos\first_glfw\first_glfw\stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\chalni_mesh.h>
//#include<C:\Users\DELL\source\repos\first_glfw\first_glfw\shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int modelcounter = 0;


class Model
{
public:
	string directory;
	bool gammaCorrection;
	vector<Mesh> meshes;

	//This is a constant value, that represents the model's coordinate in original coordinates
	glm::vec3 model_coord;
	//This is a translate vector specific to each model
	glm::vec3 model_translate;
	//Actual world position of a model can be calculated by summing up above two

	//Model rotation and scale matrices , needed to render
	glm::mat4 rotation_matrix;
	glm::mat4 scale_matrix;

	Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
	{
		loadModel(path);
	}
	/*Model(char* path)
	{
		loadModel(path);
	}*/

	//Sets the model matrix and then draws the model
	void Draw(Shader& shader)
	{
		glm::mat4 model = glm::mat4( 1.0f );
		
		model = glm::translate(model , model_translate ); // translate as per "model_translate" vector
		model = rotation_matrix * model; 
		model = scale_matrix * model;

		shader.setMat4( "model", model );
		
		// loops over each of the meshes to call their respective Draw function
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}
	//To be used to setup model position
	glm::vec3 CalculateModelPos() {
		float maxPos = -std::numeric_limits<float>::infinity();

		unsigned long long total_points = 0;
		model_coord = glm::vec3( 0.f, 0.f, 0.f );
		for(const Mesh &mesh : meshes) {
			total_points += mesh.vertices.size();
		}

		//Use center of mass as the model position
		for(const Mesh &mesh : meshes) {
			for (const Vertex& vertex : mesh.vertices) {
				model_coord += (1.0f / total_points) * vertex.Position;
			}
		}

		return model_coord ;
	}

	//Provides dimensions of object
	glm::vec3 getSize() {
		float maxValue = -std::numeric_limits<float>::infinity();
		float minValue = std::numeric_limits<float>::infinity();

		glm::vec3 minPos = { minValue,minValue,minValue };
		glm::vec3 maxPos = { maxValue,maxValue,maxValue };

		for (const Mesh& mesh : meshes) {
			for (const Vertex& vertex : mesh.vertices) {

				maxPos.x = max( maxPos.x, vertex.Position.x );
				minPos.x = min( minPos.x, vertex.Position.x );

			    maxPos.y = max( maxPos.y, vertex.Position.y );
				minPos.y = min( minPos.y, vertex.Position.y );

			    maxPos.z = max( maxPos.z, vertex.Position.z );
				minPos.z = min( minPos.z, vertex.Position.z );
				
			}
		}

		return maxPos - minPos;
	}
private:

	void loadModel(string path)
	{
		// Within loadModel, we use Assimp to load the model into a data structure of Assimp called a scene object which is the root object of Assimp's data interface
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		/* aiProcess_Triangulate -> if the model does not (entirely)consist of triangles, Assimp transforms all the model�s primitive shapes to triangles first.

			aiProcess_FlipUVs -> flips y-axis of texture coordinates
		*/

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
			!scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
			return;
		}
		directory = path.substr(0, path.find_last_of(' / '));
		processNode(scene->mRootNode, scene);
		model_coord = CalculateModelPos();
	}
	void processNode(aiNode* node, const aiScene* scene)
	{
		//each node contains a set of mesh indices where each index points to a specific mesh located in the scene object.

		//retrieve these mesh indices, retrieve each mesh, process each mesh, and then do this all again for each of the node�s children nodes.
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene, modelcounter));
			modelcounter++;
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}
	// translate aiMesh object to our Mesh object
	// Processing a mesh is a 3-part process: retrieve all the vertex data, retrieve the mesh�s indices, and finally retrieve the relevant material data.
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, unsigned int ModelIndex)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		glm::vec3 modelColors[] = {
			glm::vec3(0.2f, 0.3f, 0.5f),   // Color for model 0
			glm::vec3(0.6f, 0.1f, 0.2f),   // Color for model 1
			glm::vec3(0.0f, 0.7f, 0.3f),   // Color for model 2
			glm::vec3(0.8f, 0.6f, 0.2f),
			glm::vec3(0.3f, 0.3f, 0.8f),
			glm::vec3(0.4f, 0.9f, 0.2f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.5f, 0.5f, 0.5f)
		};

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex; //mesh class ko struct

			// process vertex positions, normals and texture coordinates
			glm::vec3 vector;// declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so transfer the data to this placeholder glm::vec3 first.

			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			//normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			//colors 
			//std::cout << ModelIndex << std::endl;
			vertex.Color = modelColors[ModelIndex];

			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		return Mesh(vertices, indices);
	}


};

//void draw //euta model lai copy garne function
#endif