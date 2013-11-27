#pragma once
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include "../Model/objModel.h"
#include "../Help/Math/AABB.h"

class World : public AABB {
protected:
	glm::vec2 size;

public:
	World( glm::vec2 size) : AABB( glm::vec2(0, 0), size){
		this->size = size;
	}
	~World() {

	}

	glm::mat4 getMatrix() {
		glm::mat4 mModelMatrix = glm::mat4(1.0f);
		mModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0 ));
		mModelMatrix = glm::scale(mModelMatrix, glm::vec3(size.x, size.y, 0.0));
		return mModelMatrix;
	}
};