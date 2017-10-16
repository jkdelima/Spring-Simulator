/*
 * SceneShader.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 *
 *  Edited by: Jeremy Kyle Delima for Assignment 3 CPSC 587
 *
 */

#include "SceneShader.h"
#include "lodepng.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>

using namespace std;
using namespace glm;

static float PI = 3.14159265359;

int picker = 1;
vector <vec3> vertices;
vec3 gravity;
float b = 0.005f;

//GLUquadricObj *quadratic = NULL;

struct Point {
	vec3 p, v, f;
	float m;
	bool fixed;
};

struct Spring {
	int index1, index2;
	float rest, k, damp;
};	

vector <Point> points;
vector <Spring> springs;

SceneShader::SceneShader(): Shader()
{
	_programPlane = 0;
	_planeVertexArray = -1;
	_programMesh = 0;
	_mvUniform = -1;
	_projUniform = -1;
	_zTranslation = 1.0;
	_aspectRatio = 1.0;
	_xRot = 0.0;
	_yRot = 0.0;
	lightPosition = glm::vec3(0.5, 0.5, 0.5);
}

void createModels(){
	vertices.clear();
	points.clear();
	springs.clear();

	/* 1. Mass on a spring */
	if (picker == 1) {
		Point p1, p2;
		p1.p = vec3(0.0f, 1.0f, 0.0f);
		p1.v = vec3(0.0f, 0.0f, 0.0f);
		p1.f = vec3(0.0f, 0.0f, 0.0f);
		p1.m = 1;
		p1.fixed = true;
		points.push_back(p1);
		p2.p = vec3(0.0f, 0.5f, 0.0f);
		p2.v = vec3(0.0f, 0.0f, 0.0f);
		p2.f = vec3(0.0f, 0.0f, 0.0f);
		p2.m = 1;
		p2.fixed = false;
		points.push_back(p2);
		Spring s1;
		s1.index1 = 0;
		s1.index2 = s1.index1 + 1;
		s1.k = 50;
		s1.rest = 0.1f;
		s1.damp = 1;
		springs.push_back(s1);
	}

	/* 2. Pendulum */
	if (picker == 2) {
		int level = 4;
		float inc = 0.2f, incX = 0.2f, incY = 0.2f;
		Point p1;
		p1.p = vec3(0.0f, 1.0f, 0.0f);
		p1.v = vec3(0.0f, 0.0f, 0.0f);
		p1.f = vec3(0.0f, 0.0f, 0.0f);
		p1.m = 1;
		p1.fixed = true;
		points.push_back(p1);
		for (int i = 0; i < level; i++){
			Point p;
			p.p = vec3(0.0f + incX, 1.0f - incY, 0.0f);
			p.v = vec3(0.0f, 0.0f, 0.0f);
			p.f = vec3(0.0f, 0.0f, 0.0f);
			p.m = 5;
			p.fixed = false;
			points.push_back(p);
			incX += inc;
			incY += inc;
		}
		for (int i = 0; i < points.size() - 1; i++) {
			Spring s;
			s.index1 = i;
			s.index2 = i + 1;
			s.k = 500;
			s.rest = 0.1f;
			s.damp = 1;
			springs.push_back(s);
		}
	}

	/* 3. Jello cube */
	if (picker == 3) {
		int level = 4;
		float inc = 0.15f, incX = 0.0f, incY = 0.0f, incZ = 0.0f;
		for (int i = 0; i < pow(level, 3); i++){
			Point p;
			p.p = vec3(-0.2f + incX, 1.0f - incY, 0.0f + incZ);
			p.v = vec3(0.0f, 0.0f, 0.0f);
			p.f = vec3(0.0f, 0.0f, 0.0f);
			p.m = 1;
			p.fixed = false;
			points.push_back(p);
			incX += inc;
			if (incX >= inc * level) {
				incX = 0.0f;
				incY += inc;
				if (incY >= inc * level) {
					incY = 0.0f;
					incZ += inc;
				}
			}
		}
		for (int i = 0; i < points.size() - 1; i++) {
			for (int j = i + 1; j < points.size(); j++) {
				Point& p1 = points.at(i);
				Point& p2 = points.at(j);
				float x = distance(p1.p, p2.p);
				if (x <= sqrt(0.07) && x != 0){
					Spring s;
					s.index1 = i;
					s.index2 = j;
					s.k = 500;
					s.rest = 0.25f;
					s.damp = 1;
					springs.push_back(s);
				}
			}
		}
	}

	/* 4. Hanging cloth */
	if (picker == 4) {
		int level = 8;
		float inc = 0.1f, incX = 0.0f, incY = 0.0f, incZ = 0.0f;
		for (int i = 0; i < pow(level, 2); i++){
			Point p;
			p.p = vec3(-0.3f + incX, 1.0f - incY, 0.0f + incZ);
			p.v = vec3(0.0f, 0.0f, 0.0f);
			p.f = vec3(0.0f, 0.0f, 0.0f);
			p.m = 1;
			if (points.size() < level) p.fixed = true;
			else p.fixed = false;
			points.push_back(p);
			incX += inc;
			if (incX >= inc * level) {
				incX = 0.0f;
				incY += inc;
			}
		}
		for (int i = 0; i < points.size() - 1; i++) {
			for (int j = i + 1; j < points.size(); j++) {
				Point& p1 = points.at(i);
				Point& p2 = points.at(j);
				float x = distance(p1.p, p2.p);
				if (x <= sqrt(0.025) && x != 0){
					Spring s;
					s.index1 = i;
					s.index2 = j;
					s.k = 500;
					s.rest = 0.1f;
					s.damp = 1;
					springs.push_back(s);
				}
			}
		}
	}

	/* 5. Flag in the wind */
	if (picker == 5) {
		int level = 4;
		float inc = 0.1f, incX = 0.0f, incY = 0.0f, incZ = 0.0f;
		for (int i = 0; i < pow(level, 2) + (level * 2); i++){
			Point p;
			p.p = vec3(-0.3f + incX, 1.0f - incY, 0.0f + incZ);
			p.v = vec3(0.0f, 0.0f, 0.0f);
			p.f = vec3(0.0f, 0.0f, 0.0f);
			p.m = 1;
			if (points.size() % (level + 2) == 0) p.fixed = true;
			else p.fixed = false;
			points.push_back(p);
			incX += inc;
			if (incX >= inc * (level + 2)) {
				incX = 0.0f;
				incY += inc;
			}
		}
		for (int i = 0; i < points.size() - 1; i++) {
			for (int j = i + 1; j < points.size(); j++) {
				Point& p1 = points.at(i);
				Point& p2 = points.at(j);
				float x = distance(p1.p, p2.p);
				if (x <= sqrt(0.025) && x != 0){
					Spring s;
					s.index1 = i;
					s.index2 = j;
					s.k = 1000;
					s.rest = 0.1f;
					s.damp = 1;
					springs.push_back(s);
				}
			}
		}
		/* Flag pole */
		Point p;
		p.p = vec3(-0.3f, 0.0f, 0.0f);
		p.v = vec3(0.0f, 0.0f, 0.0f);
		p.f = vec3(0.0f, 0.0f, 0.0f);
		p.m = 1;
		p.fixed = true;
		points.push_back(p);
		Spring s;
		s.index1 = 0;
		s.index2 = points.size() - 1;
		s.k = 1000;
		s.rest = 0.1f;
		s.damp = 1;
		springs.push_back(s);
	}
}

/* Using Semi-Implicit Euler */
void SceneShader::refresh(){

	float time = 0.01f;

	for (int i = 0; i < springs.size(); i++) {
		Point& p1 = points.at(springs.at(i).index1);
		Point& p2 = points.at(springs.at(i).index2);

		float k = springs.at(i).k;
		float x = distance(p1.p, p2.p);
		float rest = springs.at(i).rest;

		vec3 springVec = normalize(p1.p - p2.p);
		vec3 damp = springs.at(i).damp * (p1.v - p2.v);
		vec3 force = springVec * k * (x - rest) + damp;

		p1.f = p1.f - force;
		p2.f = p2.f + force;
	}

	for (int i = 0; i < points.size(); i++) {
		if (picker == 5) gravity = vec3(9.81f, -9.81f, 0.0f);
		else gravity = vec3(0.0f, -9.81f, 0.0f);
		Point& p = points.at(i);
		if (p.fixed == false) {
			vec3 damp = -b * p.v;
			p.f = p.f + gravity + damp;
			p.v = p.v + ((p.f/p.m) * time);
			p.p = p.p + (p.v * time);
			if (p.p.y <= 0.0f) {
				p.p.y = 0.0f;
				p.v = -p.v;
			}
		}
		p.f = vec3(0.0f, 0.0f, 0.0f);
	}

	vertices.clear();

	for (int i = 0; i < springs.size(); i++) {
		vertices.push_back(points.at(springs.at(i).index1).p);
		vertices.push_back(points.at(springs.at(i).index2).p);
	}

	glGenVertexArrays(1, &_meshVertexArray);
	glBindVertexArray(_meshVertexArray);

	glGenBuffers(1, &_meshVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	
	glDrawArrays(GL_LINES, 0, vertices.size());

	glBindVertexArray(0);

}

void SceneShader::createVertexBuffer()
{
	//create plane geometry
	static const GLfloat quadData[] =
	{
                        -1.0f, 0.0f, -1.0f,
                        -1.0f, 0.0f, 1.0f,
                        1.0f, 0.0f, -1.0f,
                        1.0f, 0.0f, 1.0f,
	};

	//passing model attributes to the GPU
	//plane
	glGenVertexArrays(1, &_planeVertexArray);
	glBindVertexArray(_planeVertexArray);

	glGenBuffers(1, &_planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof (quadData), quadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	createModels();
	//passing model attributes to the GPU
	//plane
	glGenVertexArrays(1, &_meshVertexArray);
	glBindVertexArray(_meshVertexArray);

	glGenBuffers(1, &_meshVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void SceneShader::startup()
{
	_programPlane = compile_shaders("./shaders/plane.vert", "./shaders/plane.frag");

	_programLight = compile_shaders("./shaders/light.vert", "./shaders/light.frag");

	_programMesh = compile_shaders("./shaders/mesh.vert", "./shaders/mesh.frag");

	createVertexBuffer();
}

void SceneShader::renderPlane()
{
	glBindVertexArray(_planeVertexArray);

	glUseProgram(_programPlane);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f,0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	glm::mat4 identity(1.0f);
	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	_modelview *=  rotationX;

	//Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programPlane, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

void SceneShader::renderMesh()
{
	glBindVertexArray(_meshVertexArray);

	glUseProgram(_programMesh);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f,0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	glm::mat4 identity(1.0f);
	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	_modelview *=  rotationX;

	//Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programMesh, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glDrawArrays(GL_LINES, 0, vertices.size());

	glBindVertexArray(0);
}

void SceneShader::renderLight()
{
	glUseProgram(_programLight);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot  * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    _modelview *=  rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programLight, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glPointSize(30.0f);
	glDrawArrays( GL_POINTS, 0, 1);
}

void SceneShader::render()
{
	renderPlane();
	renderLight();
	renderMesh();
	refresh();
}

void SceneShader::setZTranslation(float z)
{
	_zTranslation = z;
}

void SceneShader::setAspectRatio(float ratio)
{
	_aspectRatio = ratio;
}

void SceneShader::setRotationX( float x )
{
	_xRot = x;
}

void SceneShader::setRotationY( float y )
{
	_yRot = y;
}

void SceneShader::shutdown()
{
	glDeleteBuffers(1, &_meshVertexBuffer);
	glDeleteBuffers(1, &_planeVertexBuffer);
	glDeleteVertexArrays(1, &_meshVertexArray);
	glDeleteVertexArrays(1, &_planeVertexArray);
}

void SceneShader::updateLightPositionX(float x)
{
	lightPosition.x += x;
}

void SceneShader::updateLightPositionY(float y)
{
	lightPosition.y += y;
}

void SceneShader::updateLightPositionZ(float z)
{
	lightPosition.z += z;
}

void SceneShader::updatePicker(int x)
{
	picker = x;
}

SceneShader::~SceneShader()
{
	shutdown();
}
