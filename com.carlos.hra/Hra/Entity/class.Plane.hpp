/** @file class.Plane.cpp
* Trieda obsahuje funkcie na urcovanie pozicie a spravania lietadla v ramci sveta.
*/

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Help/Math/AABB.h"
#include "../../../com.carlos.architecture/entities/entities.h"
#include <log4cpp.h>

using namespace Architecture;


class Plane : public AABB {
private:
	//ControllerCommands lastCommand;
	log4cpp::Category* log;
	int mTyp; /**< Typ lietadla */

protected:
	glm::vec3 rotation; /**< Premenna ktora urcuje otacanie lietadla */
	glm::vec2 silaPohybu; /**< Rychlost stupania lietadla a klesania lietadla*/
	glm::vec2 speed; /**< Rychlost do predu a zadu */
	glm::vec2 akceleracia; /**< Velkost akceleracie pri stupani a pade*/
	glm::vec2 size; /**< Velkost lietadla */

public:
	glm::vec2 getsize(); 
	Plane(glm::vec2 size, int typ);


	/** 
	* Funkcia nema na vstupe ziadne parametre, sluzi na nastavenie startovaecj pozicie lietadla.
	* @see void PlayingGameState::switchOn(IGameState* predchodca) 
	* @return void
	*/
	void setStartPosition();

	void setRotiation(glm::vec3 rotation);

	glm::vec3 getRotation();
	int getTyp();

	/** 
	* Funkcia ma na vstupe dva parametre zmenu casu a stlacenu klavesu, jej ulohou je starat sa o hernu logiku.
	* @param fDelta - zmena casu
	* @param pressedKey - stlacena klavesa
	* @see Scene::frame(float fDelta)
	* @return void
	*/
	void logic(float fDelta, ControllerCommands pressedKey);

	/** 
	* Funkcia ma na vstupe jeden parameter, stara sa o nastavenie pozicie a natocenia liedala
	* @param pozicia - pozicia kde ma byt zobrazene lietadlo
	* @see void PlayingGameState::frame(FrameData* frame)
	* @return void
	*/
	glm::mat4 getMatrix(int pozicia);
};