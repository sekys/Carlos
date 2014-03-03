#pragma once
/** @file class.VisualController.hpp
* Trieda ktora sa stara o nacitanie shaderov o nastavenie kamery a o zobrazenie modelu lietadla.
*/

#include <stdlib.h>
#include "../Model/objModel.h"
#include "../Model/shaders.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class VisualController {
private:
	CShaderProgram control; 

	glm::mat4 projekcnaMatica;
	glm::mat4 mView;
	glm::mat4 mModelMatrix;
	glm::mat4 normalMatrix;

public:
	VisualController();

	~VisualController();
	/** 
	* Funkcia ktora sa stara o nacitanie shaderov
	* @param shaders[11] - pole shaderov
	* @see Scene::init()
	* @return void
	*/
	void load(CShader shaders[11]);
	/** 
	* Funkcia ktora zabezpecuje nastavenie matic a kamery
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void setPerspektive();

	/** 
	* Funkcia na zobrazenie lietadla ma na vstupe 2 parametre
	* @param model - 3d model lietadla 
	* @param mModelMatrix 
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void renderObject(CObjModel& model, glm::mat4& mModelMatrix);
};