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
	CTexture loadTexture(string path);

public:
	CShader shaders[11]; /**< Premenna v ktorej su ulozene shadery. */
	CObjModel plain; /**< Model lietadla */
	CObjModel square; /**< Pozadie za lietadlom, stena na ktorej sa zobraju textura. */

	CTexture bgBratislavsky; /**< Pozadie ktora sa zobrazovalo namiesto videa*/
	CTexture bgScore; /**< Textura na ktorej sa zobrazuje skore*/
	CTexture bgUvod; /**< Textura s uvodnom obrazovkou*/
	CTexture bgGameOver; /**< Textura s hlasko Game over*/

	ResourceManager();
	~ResourceManager();

	/** 
	* Funkcia ktora sa stara o nacitanie 3D objektov - lietadla, pozadia a taktiez o nacitanie ostatnych textur.
	* @param 
	* @see Scene::init()
	* @return void
	*/
	void load();

};