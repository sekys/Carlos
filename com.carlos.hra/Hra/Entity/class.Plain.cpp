#include "class.Plain.hpp"



glm::vec2 Plain::getsize(){
	return size;
}

Plain::Plain(glm::vec2 size, int typ) : AABB(size) {
	log = CREATE_LOG4CPP();
	this->lastCommand = ControllerCommands::NO_ACTION;
	this->size = size;

	setStartPosition(typ);
}

void  Plain::setStartPosition(int typ) {
	/// Hranice  sveta su -100, 100
	setPosition( glm::vec2(150, 150) ); /// Starovacia pozicia lietadla
	//rotation = glm::vec3(-270, 180, 270);
	if (typ == 1)rotation = glm::vec3(-270, 180, 270);
	if (typ == 2)rotation = glm::vec3(-270, 180, 270);
	if (typ == 3)rotation = glm::vec3(0, -90, 0);

	silaPohybu = glm::vec2(350.0f, 30.0f); /// Defaultna rychlost stupania a rychlost klesania - pri pade
	speed = glm::vec2();
	akceleracia = glm::vec2(0.0f, -10.0f);
}



void  Plain::setRotiation(glm::vec3 rotation) {
	this->rotation = rotation;
}
glm::vec3  Plain::getRotation() {
	return rotation;
}

void  Plain::logic(float fDelta, ControllerCommands pressedKey) 
{
	float radians = 1.0 * 0.0174532925f;
	glm::vec2 position = getPosition();

	this->lastCommand = pressedKey;

	/// Sila posobi na lietadlo
	glm::vec2 F = glm::vec2(0.0);
	glm::vec2 Fg = glm::vec2(0.0f, -3.0f);
	F += Fg;

	if (pressedKey == ControllerCommands::UP) {
		F.y += silaPohybu.x;
	}
	if (pressedKey == ControllerCommands::DOWN) {
		F.y += silaPohybu.y * -1.0f;
	}
	if (pressedKey == ControllerCommands::RIGHT) {
		F.x += silaPohybu.x;
	}
	if (pressedKey == ControllerCommands::LEFT) {
		F.x += silaPohybu.x * -1.0f;
	}

	if(log != NULL) {
		log->debugStream() << pressedKey;
	}

	/// Vzorce na fyziku f = m* a		a = f / m		m = 1kg
	akceleracia = F;
	/// v = v0 + s.t + 1/2 * a * t^2
	speed += akceleracia * fDelta;
	position += speed * fDelta;
	setPosition(position);
}


glm::mat4  Plain::getMatrix() {
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

ControllerCommands Plain::getLastCommand() {
	return lastCommand;
}

void Plain::setLastCommand(ControllerCommands command) {
	this->lastCommand = command;
}
