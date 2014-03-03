#include "class.VisualController.hpp"

VisualController::VisualController() {
	glm::vec3 vEye, vView, vUp;
	vEye = glm::vec3(0.001f, 0.0f, 4998.0f);
	vView = glm::vec3(0.0f, 0.0, 0.0f);
	vUp = glm::vec3(0.0f, 1.0f, 0.0f);

	projekcnaMatica = glm::ortho(-320.0f,320.0f,-240.0f,240.0f,0.1f,5000.0f);
	mView = glm::lookAt(vEye, vView, vUp);
	mModelMatrix = glm::translate(glm::mat4(1.0f), vEye);
	normalMatrix = glm::transpose(glm::inverse(mView*mModelMatrix));
}
VisualController::~VisualController() {
	control.deleteProgram();
}

void VisualController::load(CShader shaders[11]) {
	// Create shader programs
	control.createProgram();
	control.addShaderToProgram(&shaders[0]);
	control.addShaderToProgram(&shaders[1]);
	control.addShaderToProgram(&shaders[2]);
	control.addShaderToProgram(&shaders[6]);
	control.addShaderToProgram(&shaders[7]);
	control.linkProgram();
}

void VisualController::setPerspektive() {
	control.useProgram();
	control.setUniform("matrices.projMatrix", &projekcnaMatica);
	control.setUniform("gSampler", 0);
	control.setUniform("matrices.viewMatrix", &mView);
	control.setUniform("matrices.modelMatrix", &mModelMatrix);
	control.setUniform("matrices.normalMatrix", &normalMatrix);
}

void VisualController::renderObject(CObjModel& model, glm::mat4& mModelMatrix) {
	control.setUniform("matrices.normalMatrix", glm::transpose(glm::inverse(mModelMatrix)));
	control.setUniform("matrices.modelMatrix", &mModelMatrix);
	model.renderModel();
}
