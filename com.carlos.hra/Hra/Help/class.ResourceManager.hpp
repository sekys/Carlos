#pragma once
/** @file class.ResourceManager.hpp
* Trieda ktora sa stara o nacitanie  modelov.
*/

#include <stdlib.h>
#include "../Model/objModel.h"
#include "../Model/shaders.h"

using namespace std;

class ResourceManager {
private:
	/** 
	* Funkcia ma na vstupe jeden parameter a stara sa o nacitanie obrazku.
	* @param path - cesta k suboru - obrazku
	* @see void load()
	* @return CTexture
	*/
	CTexture loadTexture(string path) {
		CTexture texture;
		bool test;
		test = texture.loadTexture2D(path, true);
		if (test == false){
			throw std::exception("Nenasiel sa obrazok!\n");
		}
		texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST_MIPMAP);
		return texture;
	}

public:
	CShader shaders[11]; /**< Premenna v ktorej su ulozene shadery. */
	CObjModel plain; /**< Model lietadla */
	CObjModel square; /**< Pozadie za lietadlom, stena na ktorej sa zobraju textura. */

	CTexture bgBratislavsky; /**< Pozadie ktora sa zobrazovalo namiesto videa*/
	CTexture bgScore; /**< Textura na ktorej sa zobrazuje skore*/
	CTexture bgUvod; /**< Textura s uvodnom obrazovkou*/
	CTexture bgGameOver; /**< Textura s hlasko Game over*/

	ResourceManager() {

	}

	~ResourceManager() {
		bgBratislavsky.releaseTexture();
		bgScore.releaseTexture();
		bgUvod.releaseTexture();
		bgGameOver.releaseTexture();

		plain.releaseModel();
		square.releaseModel();
		for(int i=0; i < 11; i++) {
			shaders[i].deleteShader();
		}
	}

	/** 
	* Funkcia ktora sa stara o nacitanie 3D objektov - lietadla, pozadia a taktiez o nacitanie ostatnych textur.
	* @param 
	* @see Scene::init()
	* @return void
	*/
	void load() {
		string sShaderFileNames[11] = {"main_shader.vert", "main_shader.geom", "main_shader.frag", "ortho2D.vert",
			"ortho2D.frag", "font2D.frag", "dirLight.frag", "pointLight.frag", "fog.frag", "color.vert", "color.frag"};

		int i;
		for (i=0; i < 11; i++) {
			string sExt = sShaderFileNames[i].substr(ESZ(sShaderFileNames[i])-4, 4);
			int iShaderType = sExt == "vert" ? GL_VERTEX_SHADER : (sExt == "frag" ? GL_FRAGMENT_SHADER : GL_GEOMETRY_SHADER);
			shaders[i].loadShader("../com.carlos.hra/data/shaders/" + sShaderFileNames[i], iShaderType);
		}

		/// Load 3D models
		plain.loadModel("../com.carlos.hra/data/Nighthawk/F-117_Nighthawk.obj", "F-117_Nighthawk.mtl");
		square.loadModel("../com.carlos.hra/data/square.obj", "");

		/// Load textury pre hru
		bgBratislavsky = loadTexture("../com.carlos.hra/data/bratislavsky.png");
		bgScore = loadTexture("../com.carlos.hra/data/Carlos_Score.png");
		bgUvod = loadTexture("../com.carlos.hra/data/Carlos_uvod.png");
		bgGameOver = loadTexture("../com.carlos.hra/data/Carlos_game_over.png");
	}

};