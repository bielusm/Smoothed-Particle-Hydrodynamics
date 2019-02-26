#include "common.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "icosphere.h"
#include <iostream>
const char *WINDOW_TITLE = "Q1";
const double FRAME_RATE_MS = 1000.0/60.0;

typedef glm::vec4  color4;
typedef glm::vec4  point4;

GLuint  ModelView, Projection;

//----------------------------------------------------------------------------
std::vector<point4> pointVertices;
std::vector<GLuint> pointIndices;

// OpenGL initialization
void
init()
{
	icosphere(3, pointVertices, pointIndices);

   // Create a vertex array object
   GLuint vao = 0;
   glGenVertexArrays( 1, &vao );
   glBindVertexArray( vao );

   GLuint pointBuffer;

   // Create and initialize a buffer object
   glGenBuffers( 1, &pointBuffer);
   glBindBuffer( GL_ARRAY_BUFFER, pointBuffer );
   glBufferData( GL_ARRAY_BUFFER, pointVertices.size()*sizeof(point4), &pointVertices.front(), GL_STATIC_DRAW );

   // Another for the index buffer
   glGenBuffers( 1, &pointBuffer);
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pointBuffer);
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, pointIndices.size()*sizeof(GLuint), &pointIndices.front(), GL_STATIC_DRAW );

   // Load shaders and use the resulting shader program
   GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
   glUseProgram( program );

   // set up vertex arrays
   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

   ModelView = glGetUniformLocation( program, "ModelView" );
   Projection = glGetUniformLocation( program, "Projection" );

   glEnable( GL_DEPTH_TEST );
   glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   //  Generate the model-view matrix

   const glm::vec3 viewer_pos( 0.0, 0.0, 2.0 );
   glm::mat4 trans, rot, scale, model_view;
   trans = glm::translate(trans, -viewer_pos);
   scale = glm::scale(glm::mat4(), glm::vec3(0.03, 0.03, 0.03));
   model_view = trans * scale;
   glUniformMatrix4fv( ModelView, 1, GL_FALSE, glm::value_ptr(model_view) );
   glDrawElements(GL_TRIANGLES, pointIndices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

   glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
       case 033: // Escape Key
       case 'q': case 'Q':
          exit( EXIT_SUCCESS );
          break;
    }
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
	std::cout << "x: " << x << "y: " << y << std::endl;
  
}

//----------------------------------------------------------------------------

void
update( void )
{

}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
   glViewport( 0, 0, width, height );

   GLfloat aspect = GLfloat(width)/height;
   glm::mat4  projection = glm::perspective( glm::radians(45.0f), aspect, 0.5f, 3.0f );

   glUniformMatrix4fv( Projection, 1, GL_FALSE, glm::value_ptr(projection) );
}
