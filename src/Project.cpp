//Particle code adapted from http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/

#include "common.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>
#include "ParticleContainer.h"
#include "Particle.h" //for hval

#define MAXPARTICLES 200


static const GLfloat quad[]= {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	 0.5f, 0.5f, -0.5f,
};


const char *WINDOW_TITLE = "Project";
const int FRAME_RATE_MS = (int)(1000.0/60.0);


GLuint vertexBuffer, positionBuffer;
GLuint  ModelView, Projection, Color;

GLuint vData, vPos, Size;

GLuint vao;

ParticleContainer particles(MAXPARTICLES);
int prevTimeMs;
// OpenGL initialization
void
init()
{
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

   glGenVertexArrays( 1, &vao);
   glBindVertexArray(vao);

   glGenBuffers( 1, &vertexBuffer);
   glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer);
   glBufferData( GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW );
   vData = glGetAttribLocation(program, "vData");
   glEnableVertexAttribArray(vData);
   glVertexAttribPointer(vData, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
 
   Color = glGetUniformLocation(program, "Color");

   Size = glGetUniformLocation(program, "Size");
   glUniform1f(Size, particleSize);
   ModelView = glGetUniformLocation( program, "ModelView" );
   Projection = glGetUniformLocation( program, "Projection" );

   glEnable( GL_DEPTH_TEST );
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glm::vec3 viewer_pos(0, 0, -5);
   glm::mat4 model_view;
   model_view = glm::translate(glm::mat4(),(viewer_pos)) * glm::scale(glm::mat4(), glm::vec3(0.5f,0.5f,0.5f)) * glm::scale(glm::mat4(), glm::vec3(particleSize,particleSize,particleSize));
   glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
   glm::vec4 color(0.0f, 0.0f, 1.0f, 1.0f);
   glUniform4fv(Color, 1, glm::value_ptr(color));



   glGenBuffers(1, &positionBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
   glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
   vPos = glGetAttribLocation(program, "vPos");
   glEnableVertexAttribArray(vPos);
   glVertexAttribPointer(vPos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
   
   prevTimeMs = glutGet(GLUT_ELAPSED_TIME);

}


void
update(void)
{
	int currTimeMs = glutGet(GLUT_ELAPSED_TIME);
	int dt = currTimeMs - prevTimeMs;
	particles.updateParticles(dt);

}


void
display( void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::vector<GLfloat> positions;
	particles.getPositions(positions);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), &positions.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribDivisor(vData, 0);
	glVertexAttribDivisor(vPos, 1);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, MAXPARTICLES);
   glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{

}

//----------------------------------------------------------------------------
int hitIndex = -1;

void
mouse(int button, int state, int x, int y)
{

}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
   glViewport( 0, 0, width, height );

   GLfloat aspect = GLfloat(width)/height;
   glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.5f, 6.0f);

   glUniformMatrix4fv(Projection, 1, GL_FALSE, glm::value_ptr(projection));
}
