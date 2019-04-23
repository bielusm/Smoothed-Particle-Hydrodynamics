#pragma once
#include "Particle.h"
#include "common.h"
#include <vector>
#include "glm\glm.hpp"
#include "Grid.h"
class ParticleContainer
{
public:
	int MAXPARTICLES;
	void updateParticles(float dt);
	void findNeighbors(Particle &p, int pIndex);
	ParticleContainer(int MAXPARTICLES);
	void getPositions(std::vector<GLfloat> &positions);
	~ParticleContainer();
	std::vector <Particle> particles;

	Grid *grid;
};

