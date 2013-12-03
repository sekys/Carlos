#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "collisionStatus.h"
// https://github.com/pabennett/glblox/tree/master/lib

class AABB
{ 
protected:
	// Variables
	glm::vec2 extents;
	glm::vec2 centre;
	glm::vec2 max;    // Computed from extents and centre
	glm::vec2 min;    // Computed from extents and centre

public:      
	// Default Construct
	AABB();
	// Construct
	AABB(glm::vec2, glm::vec2); 
	AABB(glm::vec2); 

	// Access
	glm::vec2 minVector();
	glm::vec2 maxVector();

	// Modifiers
	void setPosition(glm::vec2);
	glm::vec2 getPosition();

	// Collision tests
	collisionStatus collisionTest(AABB);
	bool leftCollisionTest(AABB);
	bool rightCollisionTest(AABB);  
	bool bottomCollisionTest(AABB);
	bool topCollisionTest(AABB);  
	bool contains(AABB);
	bool contains(glm::vec2);
};