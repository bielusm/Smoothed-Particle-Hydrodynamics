#include "ParticleContainer.h"
#include "glm\gtx\norm.hpp"
#define lambda 0.4
#define BUFFER 0.5f

ParticleContainer::ParticleContainer(int MAXPARTICLES)
	: MAXPARTICLES(MAXPARTICLES)
{
	grid = new Grid(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
	float neg = 1;

	int index = 0;

	for (int i = 0; i < MAXPARTICLES / 10; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			float x = -5.0f - (i*(particleSize + BUFFER));
			float y = 2.0f +(j*(particleSize+ BUFFER)) - (i*(particleSize + BUFFER));
			glm::vec3 pos(x, y, 1);
			glm::vec3 vel(-50.0f, -50.0f, 0);
			particles.push_back(Particle(pos, vel, index, 1.0f));
			index++;
		}
	}
	for (int i = 0; i < MAXPARTICLES / 10; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			float x = 5.0f + (i*(particleSize + BUFFER));
			float y = 2.0f + (j*(particleSize + BUFFER)) - (i*(particleSize + BUFFER));
			glm::vec3 pos(x, y, 1);
			glm::vec3 vel(50.0f, -50.0f, 0);
			particles.push_back(Particle(pos, vel, index, 1.0f));
			index++;
		}
	}

	grid->makeGrid(particles);

}


void ParticleContainer::updateParticles(float dt)
{
	while (dt > 0)
	{
		int i = 0;
		float maxVal2 = 0.0000000001;
		for (Particle &p : particles)
		{
			p.clear();
			findNeighbors(p, i);
			i++;
			float velLen2 = glm::length2(p.localVelocity);
			if (velLen2 > maxVal2)
				maxVal2 = velLen2;
		}
		float maxVal = sqrt(maxVal2);
		for (Particle &p : particles)
		{
			p.CalcPressure();
		}
		for (Particle &p : particles)
		{
			p.CalcForces();
		}

		for (Particle &p : particles)
		{
			//under alg 1
			float CFL = lambda * (hVal / maxVal);
			float stepVal;
			if (dt > CFL)
			{
				stepVal = CFL;
			//	dt -= CFL;
				dt = -1.0f;
			}
			else
			{
				stepVal = dt;
				dt = -1.0f;
			}
			stepVal = stepVal / 1000.0f; //sec to milisec
			p.CalcVelocity(stepVal);
			p.CalcPosition(stepVal);
			glm::ivec3 gc  = grid->updateCoord(p.index, p.gridCoords, p.pos);
			p.gridCoords = gc;
		}
	}
}

void ParticleContainer::findNeighbors(Particle &p, int pIndex)
{
	std::vector<int> indices = grid->neighborIndices(p);
	for (int i = 0; i < indices.size(); i++)
	{
			Particle *j = &particles[indices[i]];
			if (glm::distance(p.pos, j->pos) < hVal)
			{
				p.addNeighbor(j);
			}

	}
}

void ParticleContainer::getPositions(std::vector<GLfloat> &positions)
{
	for (int i = 0; i < MAXPARTICLES; i++)
	{
		Particle p = particles[i];
		positions.push_back(p.pos.x);
		positions.push_back(p.pos.y);		
		positions.push_back(p.pos.z);
	}
}
ParticleContainer::~ParticleContainer()
{
	delete grid;
}


