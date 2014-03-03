#include "class.ResourceManager.hpp"

CTexture ResourceManager::loadTexture(string path) {
	CTexture texture;
	bool test;
	test = texture.loadTexture2D(path, true);
	if (test == false){
		throw std::exception("Nenasiel sa obrazok!\n");
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

	plain.releaseModel();
	square.releaseModel();
	for(int i=0; i < 11; i++) {
		shaders[i].deleteShader();
	}
}

void ResourceManager::load() {
	string sShaderFileNames[11] = {"main_shader.vert", "main_shader.geom", "main_shader.frag", "ortho2D.vert",
		"ortho2D.frag", "font2D.frag", "dirLight.frag", "pointLight.frag", "fog.frag", "color.vert", "color.frag"};

	int i;
	for (i=0; i < 11; i++) {
		string sExt = sShaderFileNames[i].substr(ESZ(sShaderFileNames[i])-4, 4);
		int iShaderType = sExt == "vert" ? GL_VERTEX_SHADER : (sExt == "frag" ? GL_FRAGMENT_SHADER : GL_GEOMETRY_SHADER);
		shaders[i].loadShader("../data/shaders/" + sShaderFileNames[i], iShaderType);
	}

	/// Load 3D models
	plain.loadModel("../data/models/Nighthawk/F-117_Nighthawk.obj", "F-117_Nighthawk.mtl");
	square.loadModel("../data/models/square.obj", "");

	/// Load textury pre hru
	bgScore = loadTexture("../data/screens/Carlos_Score.png");
	bgUvod = loadTexture("../data/screens/Carlos_uvod.png");
	bgGameOver = loadTexture("../data/screens/Carlos_game_over.png");
}

