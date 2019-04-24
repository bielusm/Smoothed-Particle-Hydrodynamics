#include "ParticleContainer.h"
#include "glm\gtx\norm.hpp"
#include <iostream>
#define lambda 0.4
#define BUFFER 0.3f

ParticleContainer::ParticleContainer(int MAXPARTICLES)
	: MAXPARTICLES(MAXPARTICLES)
{
	grid = new Grid(-500.0f, 500.0f, -500.0f, 500.0f);
	float neg = 1;

	int index = 0;

	//for (int i = 0; i < (MAXPARTICLES/2/20); i++)
	//{

	//	for (int j = 0; j < 20; j++)
	//	{
	//		float x = 0.0f + (j*(particleSize + BUFFER*4));
	//		float y = 10.0f+ (i*(particleSize + BUFFER));
	//		glm::vec3 pos(x, y, 1);
	//		glm::vec3 vel(0.0f, 0.0f, 0);
	//		particles.push_back(Particle(pos, vel, index, 1.0f));
	//		index++;
	//	}
	//}
	for (int i = 0; i < MAXPARTICLES / 2/2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			float x = -20.0f - (i*(particleSize + BUFFER));
			float y = 2.0f + (j*(particleSize + BUFFER)) - (i*(particleSize + BUFFER));
			glm::vec3 pos(x, y, 1);
			glm::vec3 vel(100.0f, 100.0f, 0);
			particles.push_back(Particle(pos, vel, index, 1.0f));
			index++;
		}
	}

	for (int i = 0; i < MAXPARTICLES /2/2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			float x = 20.0f + (i*(particleSize + BUFFER));
			float y = 2.0f + (j*(particleSize + BUFFER)) - (i*(particleSize + BUFFER));
			glm::vec3 pos(x, y, 1);
			glm::vec3 vel(-100.0f, 100.0f, 0);
			particles.push_back(Particle(pos, vel, index, 1.0f));
			index++;
		}
	}

	grid->makeGrid(particles);

}


void ParticleContainer::updateParticles(float dt)
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
		float  maxVal = sqrt(maxVal2);
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
		stepVal = 0.001f;
		float index = 0;
		for (Particle &p : particles)
		{
			p.CalcImmediateVelocity(stepVal);
			index++;
		}
		for (Particle &p : particles)
		{

			p.CalcImmediateDensity(stepVal);
			p.CalcPressure();
		}

		for (Particle &p : particles)
		{
			p.fPressure();
		}
		index = 0;
		for (Particle &p : particles)
		{
			p.CalcVelocity(stepVal);
			p.CalcPosition(stepVal);
			glm::ivec2 gc  = grid->updateCoord(p.index, p.gridCoords, p.pos);
			p.gridCoords = gc;
			index++;
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
		positions.push_back(1.0f);
	}
}
ParticleContainer::~ParticleContainer()
{
	delete grid;
}


