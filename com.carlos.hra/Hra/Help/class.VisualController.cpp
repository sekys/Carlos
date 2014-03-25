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

void VisualController::pushScreenCoordinateMatrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0],viewport[2],viewport[3],viewport[1]);
	glPopAttrib();
}

void VisualController::popProjectionMatrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

void VisualController::renderTexture(CTexture &texture,
										unsigned int centerX, 
										unsigned int centerY,
										unsigned int windowWidth,
										unsigned int windowHeight) {
	glUseProgram(0);
	VisualController::pushScreenCoordinateMatrix();

	GLfloat w = texture.getWidth();
	GLfloat h = texture.getHeight();

	GLfloat x = centerX - w/2;
	GLfloat y = centerY - h/2;

	x = x < 0.0f ? 0.0f : x;
	y = y < 0.0f ? 0.0f : y;

	x = x+w > windowWidth  ? windowWidth-w  : x;
	y = y+h > windowHeight ? windowHeight-h : y;

	texture.bindTexture(GL_TEXTURE_2D);

	glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f); glVertex3f(x    , h + y,  0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + w, h + y,  0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + w, y    ,  0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x    , y    ,  0.0f);

	glEnd();

	VisualController::popProjectionMatrix();
}