// Display a cube, using glDrawElements

#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *WINDOW_TITLE = "Cube with Indices";
const double FRAME_RATE_MS = 1000.0/60.0;

typedef glm::vec4  color4;
typedef glm::vec4  point4;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[4] = {
   point4( -0.5, -0.5,  0.5, 1.0 ),
   point4( -0.5,  0.5,  0.5, 1.0 ),
   point4(  0.5,  0.5,  0.5, 1.0 ),
   point4(  0.5, -0.5,  0.5, 1.0 ),
};

GLuint indices[] = {
   // each group of 3 is a triangle face
   1, 0, 3, 1, 3, 2,
   2, 3, 7, 2, 7, 6,
   3, 0, 4, 3, 4, 7,
   6, 5, 1, 6, 1, 2,
   4, 5, 6, 4, 6, 7,
   5, 4, 0, 5, 0, 1
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

GLuint  ModelView, Projection;

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
   // Create a vertex array object
   GLuint vao = 0;
   glGenVertexArrays( 1, &vao );
   glBindVertexArray( vao );

   GLuint buffer;

   // Create and initialize a buffer object
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

   // Another for the index buffer
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

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
   glm::mat4 trans, rot, model_view;
   trans = glm::translate(trans, -viewer_pos);
   rot = glm::rotate(rot, glm::radians(Theta[Xaxis]), glm::vec3(1,0,0));
   rot = glm::rotate(rot, glm::radians(Theta[Yaxis]), glm::vec3(0,1,0));
   rot = glm::rotate(rot, glm::radians(Theta[Zaxis]), glm::vec3(0,0,1));
   model_view = trans * rot;
   
   glUniformMatrix4fv( ModelView, 1, GL_FALSE, glm::value_ptr(model_view) );

   for( int i = 0; i < sizeof(indices)/sizeof(GLuint) ; i += 3 ) {
      glDrawElements( GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)) );
   }

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
    if ( state == GLUT_DOWN ) {
       switch( button ) {
          case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
          case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
          case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
       }
    }
}

//----------------------------------------------------------------------------

void
update( void )
{
    Theta[Axis] += 0.5;

    if ( Theta[Axis] > 360.0 ) {
       Theta[Axis] -= 360.0;
    }
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
