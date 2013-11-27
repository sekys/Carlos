/** @file class.Plain.hpp
* Trieda kde je spisana logika pohybu lietadla a ovladanie.
*/

#include "../Help/class.MyShaderController.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Help/Math/AABB.h"

class Plain : public AABB {
protected:
	glm::vec3 rotation; /**< Premenna ktora urcuje otacanie lietadla */
	glm::vec2 silaPohybu; /**< Rychlost stupania lietadla */
	glm::vec2 speed;
	glm::vec2 akceleracia;
	glm::vec2 size;

public:
	Plain(glm::vec2 size) : AABB(size) {
		this->size = size;
		setStartPosition();
	}


	void setStartPosition() {
		// hranice su -100 po 100
		setPosition( glm::vec2(0, 0) ); /// startovacia pozicia lietadla
		rotation = glm::vec3(-90, 0, 270); /// uvodna rotacia lietadla 90, 0, 270
		silaPohybu = glm::vec2(500.0f, 10.0f); /// Defaultna rychlost stupania a rychlost klesania - pri pade
		speed = glm::vec2();
		akceleracia = glm::vec2(0.0f, -10.0f);
	}

	void setRotiation(glm::vec3 rotation) {
		this->rotation = rotation;
	}
	glm::vec3 getRotation() {
		return rotation;
	}

	/** 
	* Funkcia ma na vstupe dva parametre zmenu casu a stlacenu klavesu, jej ulohou je starat sa o hernu logiku.
	* @param fDelta - zmena casu
	* @param pressedKey - stlacena klavesa
	* @see Scene::frame(float fDelta)
	* @return void
	*/

	void logic(float fDelta, unsigned char pressedKey) 
	{
		float radians = 1.0 * 0.0174532925f;
		glm::vec2 position = getPosition();
		/*
		if(pressedKey == 'a') {
		position.y -= sinf(radians) *  speed * fDelta;
		position.x -= cosf(radians) *  speed  * fDelta;
		}
		*/
		/*if (pressedKey == 'd' ) {
		position.y += sinf(radians) *  speed * fDelta;
		position.x += cosf(radians) *  speed  * fDelta;
		}
		*/

		/*
		if (pressedKey == 's' ) {
		position.y += sinf(radians) *  speed * fDelta;
		position.z += cosf(radians) *  speed * fDelta;
		}
		*/

		// Sila psooby na lietadlo
		glm::vec2 F = glm::vec2(0.0);
		glm::vec2 Fg = glm::vec2(0.0f, -5.0f);
		F += Fg;
		if (pressedKey == 'w' ) {
			printf("W\n");
			F.y += silaPohybu.x;
		}
		if (pressedKey == 's' ) {
			printf("S\n");
			F.y += silaPohybu.y * -1.0;
		}
		if (pressedKey == 'd' ) {
			printf("W\n");
			F.x += silaPohybu.x;
		}
		if (pressedKey == 'a' ) {
			printf("A\n");
			F.x += silaPohybu.x * -1.0;
		}

		// f = m* a		a = f / m		m = 1kg
		akceleracia = F;
		// v = v0 + s.t + 1/2 * a * t^2
		speed += akceleracia * fDelta;
		position += speed * fDelta;
		setPosition(position);
	}

	glm::mat4 getMatrix() {
		glm::vec2 position = getPosition();
		glm::mat4 mModelMatrix = glm::mat4(1.0f);
		mModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(position.x, position.y, 0.0f) );
		/// Natocenie podla osi x
		mModelMatrix = glm::rotate(mModelMatrix, getRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
		/// Natocenie podla osi y
		mModelMatrix = glm::rotate(mModelMatrix, getRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
		/// Natocenie podla osi z
		mModelMatrix = glm::rotate(mModelMatrix, getRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
		/// Naskalovanie modelu
		mModelMatrix = glm::scale(mModelMatrix, glm::vec3(7,7,7));
		return mModelMatrix;
	}
};