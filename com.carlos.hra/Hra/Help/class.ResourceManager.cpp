#include "class.ResourceManager.hpp"
#include "../Entity/class.Plain.hpp"
CTexture ResourceManager::loadTexture(string path) {
	CTexture texture;
	bool test;
	test = texture.loadTexture2D(path, true);
	if (test == false){
		throw std::exception("Nenasiel sa obrazok!");
	}
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST_MIPMAP);
	return texture;
}

ResourceManager::ResourceManager() {

}

ResourceManager::~ResourceManager() {
	bgScore.releaseTexture();
	bgUvod.releaseTexture();
	bgGameOver.releaseTexture();
	infoImage.releaseTexture();

	plain.releaseModel();

	square.releaseModel();

	for(int i=0; i < 11; i++) {
		shaders[i].deleteShader();
	}
}

void ResourceManager::load(int typ) {
	string sShaderFileNames[11] = {"main_shader.vert", "main_shader.geom", "main_shader.frag", "ortho2D.vert",
		"ortho2D.frag", "font2D.frag", "dirLight.frag", "pointLight.frag", "fog.frag", "color.vert", "color.frag"};

	int i;
	for (i=0; i < 11; i++) {
		string sExt = sShaderFileNames[i].substr(ESZ(sShaderFileNames[i])-4, 4);
		int iShaderType = sExt == "vert" ? GL_VERTEX_SHADER : (sExt == "frag" ? GL_FRAGMENT_SHADER : GL_GEOMETRY_SHADER);
		shaders[i].loadShader("../data/shaders/" + sShaderFileNames[i], iShaderType);
	}
	
	/// Load 3D models
	if (typ == 1)plain.loadModel("../data/models/Nighthawk/F-117_Nighthawk.obj", "F-117_Nighthawk.mtl");
	if (typ == 2)plain.loadModel("../data/models/Mig-25_Foxbat/Mig-25_Foxbat/Mig-25_Foxbat.obj", "Mig-25_Foxbat.mtl");
	if (typ == 3)plain.loadModel("../data/models/A6M_ZERO/A6M_ZERO/A6M_ZERO.obj", "A6M_ZERO.mtl");

	square.loadModel("../data/models/square.obj", "");

	/// Load textury pre hru
	bgScore = loadTexture("../data/screens/Carlos_Score.png");
	bgUvod = loadTexture("../data/screens/Carlos_uvod.png");
	bgGameOver = loadTexture("../data/screens/Carlos_game_over.png");
	infoImage = loadTexture("../data/textures/info_image.png");
}

