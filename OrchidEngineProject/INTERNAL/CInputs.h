#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <thread> //Utile depuis C++11 pour utiliser sleep_for() ou sleep_until()
#include <cmath>
#include <fstream>
#include <map>

#define GLEW_STATIC //car on lie via gl32s.lib
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "GlobalTools.h"
#include "CException.h"
#include "CCamera.h"

class CEngine;

class CInputs {
private:
	glm::mat4 mat4INPMovement;
public:
	std::string strINPKeyBindsPathFile;
	double dINPDiffTime;
	CCamera camINPChosenCamera;
	int iINPCameraState;
	float fINPLastMouseX;
	float fINPLastMouseY;
	float fINPSensitivity;
	bool bINPFirstMouse;
	CInputs();
	CInputs(CCamera &camera);
	~CInputs();
	//Callbacks methods

	//Use keycallback for inputs that doesn't need to be smooth (like instant input)
	void INPKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void INPMouseCallback(GLFWwindow* window, double xpos, double ypos);
	void INPScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void INPSetMovement(glm::mat4 movement);
	void INPSetKeyBindsPathFile(std::string path);
	glm::mat4 mat4INPGetMovement();
	std::string strINPGetKeyBindsPathFile();

	//For inputs that need to be smooth (like movements input)
	void processInputs(GLFWwindow* window);

	//New variables & methods
	std::map<std::string, int> mapStrIntINPKeybinds;
	void INPAddingKeybind(std::string function_name, int key);
	void INPWriteMapBindingsOnTxtFile();
	std::string strINPMapBindingsFromFileToString();
	void INPMapBindings(std::string bindings_from_file);
};