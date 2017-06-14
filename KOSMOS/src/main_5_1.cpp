#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>


#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor;
GLuint programTexture;
GLuint programTextureProc;
GLuint programSunTexture;
GLuint g_Texture;
GLuint g1_Texture;
GLuint g2_Texture;
GLuint earth_Texture;
GLuint venus_Texture;
GLuint moon_Texture;
GLuint jupiter_Texture;
GLuint saturn_Texture;
GLuint steel_Texture;

Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;
obj::Model sphere1Model;
obj::Model sunModel;
obj::Model ringModel;
float cameraAngle = 0;

glm::vec3 cameraPos = glm::vec3(-20, -4, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

//glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));
//glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.7f, 1.0f));


void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'g': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 0, 1)) * moveSpeed; break;
	case 'v': cameraPos += glm::cross(cameraDir, glm::vec3(0, 0, 1)) * moveSpeed; break;
	}
}

glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	
	glm::vec3 up = glm::vec3(0,1,0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);
	glm::vec3 lightDir = glm::normalize(glm::vec3(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z));

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	

	
	
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	

	Core::DrawModel(model);

	glUseProgram(0);
}




void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint texture)
{
	GLuint program = programTexture;

	glUseProgram(program);

	//glUniform3f(glGetUniformLocation(program, "texture"), SetActiveTexturetexture);
	Core::SetActiveTexture(texture, "texture", program, 0);
	glm::vec3 lightDir = glm::normalize(glm::vec3(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z));

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	


	Core::DrawModel(model);

	glUseProgram(0);
}

void drawSunTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint texture)
{
	GLuint program = programSunTexture;

	glUseProgram(program);

	//glUniform3f(glGetUniformLocation(program, "texture"), SetActiveTexturetexture);
	Core::SetActiveTexture(texture, "texture", program, 0);
	glm::vec3 lightDir = glm::normalize(glm::vec3(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z));

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);


	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);



	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectProceduralTexture(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color) {
	GLuint program = programTextureProc;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glm::vec3 lightDir = glm::normalize(glm::vec3(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z));

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	
	Core::DrawModel(model);

	glUseProgram(0);
}


void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.25f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 transformation;
	glm::mat4 transformation2;
	glm::mat4 transformation3;
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float time2 = glutGet(GLUT_ELAPSED_TIME) / 1300.0f;
	float time3 = glutGet(GLUT_ELAPSED_TIME) / 1100.0f; 

	transformation[0][0] = cos(time);
	transformation[0][1] = 0;
	transformation[0][2] = -sin(time);
	transformation[1][0] = 0;
	transformation[1][1] = 1;
	transformation[1][2] = 0;
	transformation[2][0] = sin(time);
	transformation[2][1] = 0;
	transformation[2][2] = cos(time);

	transformation2[0][0] = cos(time2);
	transformation2[0][1] = 0;
	transformation2[0][2] = -sin(time2);
	transformation2[1][0] = 0;
	transformation2[1][1] = 1;
	transformation2[1][2] = 0;
	transformation2[2][0] = sin(time2);
	transformation2[2][1] = 0;
	transformation2[2][2] = cos(time2);

	transformation3[0][0] = cos(time3);
	transformation3[0][1] = 0;
	transformation3[0][2] = -sin(time3);
	transformation3[1][0] = 0;
	transformation3[1][1] = 1;
	transformation3[1][2] = 0;
	transformation3[2][0] = sin(time3);
	transformation3[2][1] = 0;
	transformation3[2][2] = cos(time3);

	drawSunTexture(&sunModel, glm::translate(glm::vec3(0, 0, 0)) * glm::scale(glm::vec3(4)) * transformation2, g1_Texture);
	
	drawObjectTexture(&sphereModel, glm::scale(glm::vec3(0.2f)) * glm::translate(glm::vec3(0, 0, 0)) * transformation * glm::translate(glm::vec3(-25.5f, 0, 0)) * transformation, venus_Texture);
	drawObjectTexture(&sphereModel, glm::scale(glm::vec3(0.3f)) * glm::translate(glm::vec3(0, 0, 0)) * transformation2 * glm::translate(glm::vec3(20, 0, 0)) * transformation2, earth_Texture);
	drawObjectTexture(&sphereModel, glm::scale(glm::vec3(0.5f)) * glm::translate(glm::vec3(0, 0, 0)) * transformation3 * glm::translate(glm::vec3(13.8f, 0, 0)) * transformation3, jupiter_Texture);
	drawSunTexture(&sphereModel, glm::scale(glm::vec3(0.1f)) * glm::translate(glm::vec3(0, 0, 0)) * transformation2 * glm::translate(glm::vec3(60, 2, 0)) * transformation * glm::translate(glm::vec3(0, 0, 0)) * transformation2 * glm::translate(glm::vec3(5, 0, 0)) * transformation, moon_Texture);
	drawObjectTexture(&sphereModel, glm::scale(glm::vec3(0.4f)) * glm::translate(glm::vec3(0, 0, 0)) * transformation * glm::translate(glm::vec3(22.5f, 0, 0)) * transformation3, saturn_Texture);
	drawSunTexture(&ringModel, glm::scale(glm::vec3(0.004f)) * glm::translate(glm::vec3(0, 0, 0)) * transformation * glm::translate(glm::vec3(2250, 0, 0)) * transformation3, moon_Texture);

	drawObjectTexture(&shipModel, shipModelMatrix , steel_Texture);
	drawSunTexture(&sphereModel, glm::translate(glm::vec3(0, 0, 0))* glm::scale(glm::vec3(50)), g2_Texture);
	//drawObjectTexture(&sunModel, glm::translate(glm::vec3(2,0,2)) * glm::scale(glm::vec3(0.7f)), g1_Texture);
	//drawObjectTexture(&sphereModel, glm::translate(glm::vec3(2, 0, 3)), g_Texture);

	//drawObjectProceduralTexture(&ringModel, glm::translate(glm::vec3(-2,0,-2)), glm::vec3(0.0,0.0,0.0));

	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);

	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programTextureProc = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	programSunTexture = shaderLoader.CreateProgram("shaders/sun_tex.vert", "shaders/sun_tex.frag");
	//programTexture = shaderLoader.CreateProgram("shaders/1.vs", "shaders/3.fs");
	//programTextureProc = shaderLoader.CreateProgram("shaders/1.vs", "shaders/3.fs");
	
	g_Texture = Core::LoadTexture("textures/grid.png");
	g2_Texture = Core::LoadTexture("textures/spacebg.png");
	g1_Texture = Core::LoadTexture("textures/sun3.png");
	earth_Texture = Core::LoadTexture("textures/earth.png");
	venus_Texture = Core::LoadTexture("textures/venus.png");
	moon_Texture = Core::LoadTexture("textures/asphalt.png");
	saturn_Texture = Core::LoadTexture("textures/saturn.png");
	jupiter_Texture = Core::LoadTexture("textures/jupiter.png");
	steel_Texture = Core::LoadTexture("textures/starwars.png");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	sphere1Model = obj::loadModelFromFile("models/sphere1.obj");
	sunModel = obj::loadModelFromFile("models/sphere.obj");
	ringModel = obj::loadModelFromFile("models/ring.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Pierwszy Program");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
