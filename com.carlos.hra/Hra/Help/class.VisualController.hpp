#pragma once
/** @file class.VisualController.hpp
* Trieda ktora sa stara o nacitanie shaderov o nastavenie kamery a o zobrazenie modelu lietadla.
*/

#include <stdlib.h>
#include "../Model/objModel.h"
#include "../Model/shaders.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class VisualController {
	CShaderProgram control; 

public:
	VisualController() {

	}

	~VisualController() {
		control.deleteProgram();
	}
	/** 
	* Funkcia ktora sa stara o nacitanie shaderov
	* @param shaders[11] - pole shaderov
	* @see Scene::init()
	* @return void
	*/
	void load(CShader shaders[11]) {
		// Create shader programs
		control.createProgram();
		control.addShaderToProgram(&shaders[0]);
		control.addShaderToProgram(&shaders[1]);
		control.addShaderToProgram(&shaders[2]);
		control.addShaderToProgram(&shaders[6]);
		control.addShaderToProgram(&shaders[7]);
		control.linkProgram();
	}

	/** 
	* Funkcia ktora zabezpecuje nastavenie matic a kamery
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void setPerspektive() {
		glm::mat4 projekcnaMatica;
		projekcnaMatica = glm::ortho(-320.0f,320.0f,-240.0f,240.0f,0.1f,5000.0f);
		control.useProgram();
		glm::vec3 vEye, vView, vUp;
		vEye = glm::vec3(0.001f, 0.0f, 4998.0f);
		vView = glm::vec3(0.0f, 0.0, 0.0f);
		vUp = glm::vec3(0.0f, 1.0f, 0.0f);
		control.setUniform("matrices.projMatrix", projekcnaMatica);
		control.setUniform("gSampler", 0);
		glm::mat4 mView = glm::lookAt(vEye, vView, vUp);
		control.setUniform("matrices.viewMatrix", &mView);
		control.setUniform("matrices.modelMatrix", glm::mat4(1.0f));
		glm::mat4 mModelMatrix = glm::translate(glm::mat4(1.0f), vEye);
		control.setUniform("matrices.modelMatrix", &mModelMatrix);
		control.setUniform("matrices.normalMatrix", glm::transpose(glm::inverse(mView*mModelMatrix)));
	}

	/** 
	* Funkcia na zobrazenie lietadla ma na vstupe 2 parametre
	* @param model - 3d model lietadla 
	* @param mModelMatrix 
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void renderObject(CObjModel model, glm::mat4 mModelMatrix) {
		control.setUniform("matrices.normalMatrix", glm::transpose(glm::inverse(mModelMatrix)));
		control.setUniform("matrices.modelMatrix", &mModelMatrix);
		model.renderModel();
	}
};