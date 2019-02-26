// Generate an icosphere
// COMP 4490 @ umanitoba.ca Winter 2018
#include "icosphere.h"

// These definitions are from the OpenGL Red Book example 2-13
#define X .525731112119133606
#define Z .850650808352039932
static GLfloat vdata[12][3] = {
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};
static GLuint tindices[20][3] = {
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
};

void icosphere(int sub, std::vector<glm::vec4> &vertices, std::vector<GLuint> &indices) {
   for (GLfloat *v: vdata) {
      vertices.push_back(glm::vec4(v[0], v[1], v[2], 1));
   }
   indices.assign((GLuint *)tindices, (GLuint *)tindices + (sizeof(tindices) / sizeof(GLuint)));
   
   // use a regular subdivision (each tri into 4 equally-sized)
   for (int s = 0; s < sub; s++) {
      int isize = indices.size();
      for (int tri = 0; tri < isize; tri += 3) {
         int i0 = indices[tri];
         int i1 = indices[tri+1];
         int i2 = indices[tri+2];

         // create midpoints and "push" them out to the unit sphere
         // (but exclude the homogeneous coordinate)
         glm::vec3 midpoint0(glm::normalize(glm::vec3(vertices[i0] + vertices[i1]) * 0.5f));
         glm::vec3 midpoint1(glm::normalize(glm::vec3(vertices[i1] + vertices[i2]) * 0.5f));
         glm::vec3 midpoint2(glm::normalize(glm::vec3(vertices[i2] + vertices[i0]) * 0.5f));

         // add the midpoints to the vertices list
         int m0 = vertices.size();
         int m1 = m0+1;
         int m2 = m0+2;
         vertices.push_back(glm::vec4(midpoint0, 1));
         vertices.push_back(glm::vec4(midpoint1, 1));
         vertices.push_back(glm::vec4(midpoint2, 1));
         
         // now the four triangles
         indices[tri+1] = m0;
         indices[tri+2] = m2;
         indices.push_back(m0);
         indices.push_back(i1);
         indices.push_back(m1);
         indices.push_back(m0);
         indices.push_back(m1);
         indices.push_back(m2);
         indices.push_back(m2);
         indices.push_back(m1);
         indices.push_back(i2);
      }
   }
}
