/*
 * SceneShader.h
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 *
 *  Edited by: Jeremy Kyle Delima
 *
 */

#ifndef SCENESHADER_H_
#define SCENESHADER_H_

#include "Shader.h"
#include <vector>

#define GLM_FORCE_RADIANS

#include "TriMesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "texture.h"

class SceneShader : public Shader
{
public:

	SceneShader();
	~SceneShader();

	void startup ();
	void shutdown ();
	void render();

	void renderPlane();
	void renderMesh();
	void renderLight();

	void setAspectRatio( float ratio );
	void setZTranslation(float z);
	void setRotationX( float x );
	void setRotationY( float y );

	void updateLightPositionX( float x );
	void updateLightPositionY( float y );
	void updateLightPositionZ( float z );

	void updatePicker( int x );

	void refresh();

private:

	/*methods*/

	void createVertexBuffer();

	//image
	std::vector<unsigned char> _image;
	unsigned int _imageWidth;
	unsigned int _imageHeight;

	/*variables*/
	GLuint _programLight;
	GLuint _programPlane;
	GLuint _programMesh;

	GLuint _planeVertexArray;
	GLuint _planeVertexBuffer;

	GLuint _meshVertexArray;
	GLuint _meshVertexBuffer;

    GLint _mvUniform, _projUniform;

	/* Matrices */
	glm::mat4 _modelview;
	glm::mat4 _projection;

	float _zTranslation;
	float _xRot;
	float _yRot;
	float _aspectRatio;

	glm::vec3 lightPosition;

};

#endif /* SCENESHADER_H_ */
