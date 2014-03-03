/** @file class.Plain.hpp
* Trieda obsahuje : funkciu na nastavenie starovacej pozicie lietadla, logika pohybu lietadla pri ovladani - fyziku.
*/

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Help/Math/AABB.h"
#include "../../../com.carlos.architecture/entities/entities.h"

using namespace Architecture;

class Plain : public AABB {
protected:
	glm::vec3 rotation; /**< Premenna ktora urcuje otacanie lietadla */
	glm::vec2 silaPohybu; /**< Rychlost stupania lietadla a klesania lietadla*/
	glm::vec2 speed; /**< Rychlost do predu a zadu */
	glm::vec2 akceleracia; /**< Velkost akceleracie pri stupani a pade*/
	glm::vec2 size; /**< Velkost lietadla */

public:
	glm::vec2 getsize();
	Plain(glm::vec2 size);

	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na nastavenie startovacej pozicie lietadla.
	* @see SPlain(glm::vec2 size) : AABB(size)
	* @return void
	*/
	void setStartPosition();

	void setRotiation(glm::vec3 rotation);
	glm::vec3 getRotation();

	/** 
	* Funkcia ma na vstupe dva parametre zmenu casu a stlacenu klavesu, jej ulohou je starat sa o hernu logiku.
	* @param fDelta - zmena casu
	* @param pressedKey - stlacena klavesa
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void logic(float fDelta, ControllerCommands pressedKey);

	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na nastavenie startovacej pozicie lietadla vo svete.
	* @see Scene::frame(float fDelta)
	* @see Scene::stavHrania(float fDelta)
	* @return void
	*/
	glm::mat4 getMatrix();
};