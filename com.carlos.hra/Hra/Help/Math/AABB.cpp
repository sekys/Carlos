/** @file AABB.cpp
* Trieda obsahuje funkcie na urcovanie pozicie lietadla v ramci sveta.
*/
#include "AABB.h"
float epsilon = 1e-1f; 
AABB::AABB()
{
}

AABB::AABB(glm::vec2 centre, glm::vec2 extents)
{              
	AABB::centre = centre;
	AABB::extents = extents;
	AABB::max = maxVector();
	AABB::min = minVector();
}
AABB::AABB(glm::vec2 extents)
{        
	AABB(glm::vec2(), extents);
}
void AABB::setPosition(glm::vec2 centre)
{
	AABB::centre = centre;
	AABB::max = maxVector();
	AABB::min = minVector();
}
glm::vec2 AABB::getPosition() {
	return AABB::centre;
}
collisionStatus AABB::collisionTest(AABB other)
{
	collisionStatus result;

	result.left = leftCollisionTest(other);
	result.right = rightCollisionTest(other);
	result.bottom = bottomCollisionTest(other);
	result.top = topCollisionTest(other);

	return result;
}

// Check this AABB against the other AABB && indicate if there is
// a collision. Indicate which face of this AABB is in contact with the other.
bool AABB::leftCollisionTest(AABB other)
{
	if(
		max.y - epsilon > other.min.y && min.y + epsilon < other.max.y &&
		min.x <= other.max.x && max.x >= other.max.x)
	{
		return true;
	}
	return false;
}

bool AABB::rightCollisionTest(AABB other)
{   
	if(
		max.y - epsilon > other.min.y && min.y + epsilon < other.max.y &&
		max.x >= other.min.x && min.x <= other.max.x)
	{
		return true;
	}
	return false;
}

bool AABB::bottomCollisionTest(AABB other)
{
	if(max.x - epsilon > other.min.x && min.x + epsilon < other.max.x &&
		min.y <= other.max.y && max.y >= other.max.y)
	{
		return true;
	}
	return false;
}

bool AABB::topCollisionTest(AABB other)
{   
	if(max.x - epsilon > other.min.x && min.x + epsilon < other.max.x &&
		max.y >= other.min.y && min.y <= other.max.y)
	{
		return true;
	}
	return false;
}

bool AABB::contains(AABB other)
{
	if(other.min.x >= min.x && other.max.x <= max.x &&
		other.min.y >= min.y && other.max.y <= max.y
	  )
	{
		return true;
	}
	return false;
}

glm::vec2 AABB::minVector()
{
	return glm::vec2( centre.x - extents.x,
		centre.y - extents.y
		);
}

glm::vec2 AABB::maxVector()
{
	return glm::vec2( centre.x + extents.x,
		centre.y + extents.y);
}
bool AABB::contains(glm::vec2 bod) {
	return (min.x < bod.x && bod.x < max.x) &&
		(min.y < bod.y && bod.y < max.y);

}