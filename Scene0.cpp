#include <glew.h>
#include <iostream>
#include "Window.h"
#include "Debug.h"
#include "Scene0.h"
#include "Camera.h"
#include "Object.h"
#include "ObjLoader.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "MMath.h"
#include "Debug.h"

Scene0::Scene0():
	camera(nullptr), gameObject(nullptr),meshPtr(nullptr),shaderPtr(nullptr),texturePtr(nullptr) {
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {}

bool Scene0::OnCreate() {
	camera = new Camera();

	if (ObjLoader::loadOBJ("meshes/Mario.obj") == false) {
		return false;
	}
	meshPtr = new Mesh(GL_TRIANGLES, ObjLoader::vertices, ObjLoader::normals, ObjLoader::uvCoords);
	shaderPtr = new Shader("phongVert.glsl", "phongFrag.glsl");
	texturePtr = new Texture();
	if (meshPtr == nullptr|| shaderPtr == nullptr || texturePtr == nullptr) {
		Debug::FatalError("Couldn't create game object assets", __FILE__, __LINE__);
		return false;
	}
	if (texturePtr->LoadImage("textures/skull_texture.jpg") == false) {
		Debug::FatalError("Couldn't load texture", __FILE__, __LINE__);
		return false;
	}

	gameObject = new Object(meshPtr, shaderPtr, texturePtr);
	if (gameObject == nullptr) {
		Debug::FatalError("GameObject could not be created", __FILE__, __LINE__);
		return false;
	}
	lightSource = Vec3(0.0, 0.0, 10.0);
	
	return true;
}

void Scene0::OnDestroy() {
	if(camera) delete camera, camera = nullptr;
	if(meshPtr) delete meshPtr, meshPtr = nullptr;
	if(texturePtr) delete texturePtr, texturePtr = nullptr;
	if(shaderPtr) delete shaderPtr, shaderPtr = nullptr;
	if(gameObject) delete gameObject, gameObject = nullptr;
}

void Scene0::HandleEvents(const SDL_Event &sdlEvent) {

}

void Scene0::Update(const float deltaTime_) {
	gameObject->Update(deltaTime_);
	static float rotation = 0.0f;
	rotation += 1.5f;
	gameObject->setModelMatrix(MMath::rotate(rotation, Vec3(1.0f, 1.0f, 1.0f))* MMath::scale(1.0f, 1.0f, 1.0f));
}

void Scene0::Render() const {
	/// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	/// Draw your scene here
	GLuint program = gameObject->getShader()->getProgram();
	glUseProgram(program);

	/// These pass the matricies and the light position to the GPU
	glUniformMatrix4fv(gameObject->getShader()->getUniformID("projectionMatrix"), 1, GL_FALSE, camera->getProjectionMatrix());
	glUniformMatrix4fv(gameObject->getShader()->getUniformID("viewMatrix"), 1, GL_FALSE, camera->getViewMatrix());
	glUniform3fv(gameObject->getShader()->getUniformID("lightPos"), 1, lightSource);

	gameObject->Render();

	glUseProgram(0);
}
