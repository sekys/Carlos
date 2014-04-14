/** @file class.World.hpp
* Trieda obsahuje : funkciu na nastavenie sveta.
*/
#pragma once
#include <stdlib.h>
#include "../Model/objModel.h"
#include "../Help/Math/AABB.h"

class World : public AABB {
protected:
	glm::vec2 size; /**< Premenna ktora urcuje velkost sveta */

public:
	World( glm::vec2 size) : AABB( glm::vec2(0, 0), size){
		this->size = size;
	}
	~World() {

	}

	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o nastavenie pozicie a natocenia sveta
	* @param position - pozicia kde ma byt zobrazeny svet
	* @see void Scene::frame(float fDelta)
	* @return void
	*/
	glm::mat4 getMatrix(int position) {
		glm::mat4 mModelMatrix = glm::mat4(1.0f);
		mModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(position, 0, 0 ));
		//mModelMatrix = glm::scale(mModelMatrix, glm::vec3(size.x, size.y, 1.0));
		mModelMatrix = glm::scale(mModelMatrix, glm::vec3(220, size.y, 1.0));
		return mModelMatrix;
	}
};