#pragma once
/** @file class.MyShaderController.hpp
* Trieda ktora sa stara o nacitanie a vykreslenie modelu lietadla.
* Taktiez aj o pripravenie samotnej sceny.
*/

#include <stdlib.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include "../Model/objModel.h"
#include "../Model/shaders.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class MyShaderController {
protected:
	CShader shaders[11]; /**< Premenna v ktorej su ulozene shadery. */

public:
	CShaderProgram control; 
	CObjModel plain; /**< Nacitany model lietadla */
	CObjModel square; /**< Pozadie za lietadlom, stena na ktorej sa zobraju textura. */

	MyShaderController() {

	}

	/** 
	* Funkcia ma na vstupe jeden parameter a to MAT strukturu
	* @param arr
	* @see load()
	* @return res - unsigned char 
	*/
	float* toArray(vector<vector<double> >& arr)
	{
		if (arr.empty())
		{
			return NULL;
		}
		else
		{
			int m = arr.size();
			int n = arr[0].size();
			float *res = new float[m * n];
			int count = 0;

			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					res[count++] = (float) arr[i][j];
				}
			}
			return res;
		}
	}


	/** 
	* Funkcia ktora sa stara o nacitanie 3D objektov - lietadla a pozadia
	* @param 
	* @see setTexture()
	* @return void
	*/
	void load() {
		string sShaderFileNames[11] = {"main_shader.vert", "main_shader.geom", "main_shader.frag", "ortho2D.vert",
			"ortho2D.frag", "font2D.frag", "dirLight.frag", "pointLight.frag", "fog.frag", "color.vert", "color.frag"};

		int i;
		for (i=0; i < 11; i++) {
			string sExt = sShaderFileNames[i].substr(ESZ(sShaderFileNames[i])-4, 4);
			int iShaderType = sExt == "vert" ? GL_VERTEX_SHADER : (sExt == "frag" ? GL_FRAGMENT_SHADER : GL_GEOMETRY_SHADER);
			shaders[i].loadShader("Data/shaders/" + sShaderFileNames[i], iShaderType);
		}

		// Create shader programs
		control.createProgram();
		control.addShaderToProgram(&shaders[0]);
		control.addShaderToProgram(&shaders[1]);
		control.addShaderToProgram(&shaders[2]);
		control.addShaderToProgram(&shaders[6]);
		control.addShaderToProgram(&shaders[7]);
		control.linkProgram();

		// Load models
		//mdlSpongeBob.loadModel("Data/Spongebob/spongebob_bind.obj", "spongebob_bind.mtl");
		plain.loadModel("Data/Nighthawk/F-117_Nighthawk.obj", "F-117_Nighthawk.mtl");
		square.loadModel("Data/square.obj", "");
	}


	/** 
	* Funkcia ktora zabezpecuje nastavenie matic a kamery
	* @param nil 
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void setPerspektive() {
		glm::mat4 projekcnaMatica;
		// projekcnaMatica = glm::perspective(45.0f, 1.0f, 0.1f, 5000.0f);
		projekcnaMatica = glm::ortho(-320.0f,320.0f,-240.0f,240.0f,0.1f,5000.0f);
		control.useProgram();
		glm::vec3 vEye, vView, vUp;
		vEye = glm::vec3(0.0001f, 0.0f, 5000.0f);
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
	* Funkcia na zobrazenie lietadla 
	* @param nil 
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void renderObject(CObjModel model, glm::mat4 mModelMatrix) {
		control.setUniform("matrices.normalMatrix", glm::transpose(glm::inverse(mModelMatrix)));
		control.setUniform("matrices.modelMatrix", &mModelMatrix);
		model.renderModel();
	}

	/** 
	* Funkcia na nastavenie textury
	* @param nil 
	* @see load()
	* @return void
	*/
	void setBackgroud(unsigned char *image, int width, int height, GLenum inputColourFormat ) {
		CTexture texture;
		texture.createFromData(image, width, height, GL_RGB, inputColourFormat, true);
		texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST_MIPMAP);
		square.setTexture(texture);
	}
	void setBackground(string path) {
		CTexture texture;
		texture.loadTexture2D(path, true);
		texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST_MIPMAP);
		square.setTexture(texture);
	}
};