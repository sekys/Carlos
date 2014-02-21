/** @file class.Plain.hpp
* Trieda obsahuje : funkciu na nastavenie starovacej pozicie lietadla, logika pohybu lietadla pri ovladani - fyziku.
*/

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Help/Math/AABB.h"
#include "../../../Carlos/architecture/entities/entities.h"

using namespace Architecture;

class Plain : public AABB {
protected:
	glm::vec3 rotation; /**< Premenna ktora urcuje otacanie lietadla */
	glm::vec2 silaPohybu; /**< Rychlost stupania lietadla a klesania lietadla*/
	glm::vec2 speed; /**< Rychlost do predu a zadu */
	glm::vec2 akceleracia; /**< Velkost akceleracie pri stupani a pade*/
	glm::vec2 size; /**< Velkost lietadla */

public:
	glm::vec2 getsize(){
		return size;
	}
	Plain(glm::vec2 size) : AABB(size) {
		this->size = size;
		setStartPosition();
	}

	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na nastavenie startovacej pozicie lietadla.
	* @see SPlain(glm::vec2 size) : AABB(size)
	* @return void
	*/
	void setStartPosition() {
		/// Hranice  sveta su -100, 100
		setPosition( glm::vec2(150, 150) ); /// Starovacia pozicia lietadla
		rotation = glm::vec3(-270, 180, 270); /// Uvodne natocenie lietadla
		silaPohybu = glm::vec2(350.0f, 30.0f); /// Defaultna rychlost stupania a rychlost klesania - pri pade
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
	void logic(float fDelta, ControllerCommands pressedKey) 
	{
		float radians = 1.0 * 0.0174532925f;
		glm::vec2 position = getPosition();
		
		/// Sila posobi na lietadlo
		glm::vec2 F = glm::vec2(0.0);
		glm::vec2 Fg = glm::vec2(0.0f, -30.0f);
		F += Fg;
		if (pressedKey == ControllerCommands::UP) {
			printf("W\n");
			F.y += silaPohybu.x;
		}
		if (pressedKey == ControllerCommands::DOWN) {
			printf("S\n");
			F.y += silaPohybu.y * -1.0;
		}
		if (pressedKey == ControllerCommands::RIGHT) {
			printf("W\n");
			F.x += silaPohybu.x;
		}
		if (pressedKey == ControllerCommands::LEFT) {
			printf("A\n");
			F.x += silaPohybu.x * -1.0;
		}

		/// Vzorce na fyziku f = m* a		a = f / m		m = 1kg
		akceleracia = F;
		/// v = v0 + s.t + 1/2 * a * t^2
		speed += akceleracia * fDelta;
		position += speed * fDelta;
		setPosition(position);
	}

	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na nastavenie startovacej pozicie lietadla vo svete.
	* @see Scene::frame(float fDelta)
	* @see Scene::stavHrania(float fDelta)
	* @return void
	*/
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