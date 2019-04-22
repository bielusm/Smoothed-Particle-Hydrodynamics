#include "ParticleContainer.h"
#include "glm\gtx\norm.hpp"
#define lambda 0.4


ParticleContainer::ParticleContainer(int MAXPARTICLES)
	: MAXPARTICLES(MAXPARTICLES)
{
	float neg = 1;


	for (int i = 0; i < MAXPARTICLES / 2; i++)
	{
		neg = -neg;
		float x = -5.0f - (i*particleSize);
		float y = 2.1f;
		glm::vec3 pos(x, y, 1);
		glm::vec3 vel(-10.0f,0.0f, 0.0f);
		particles.push_back(Particle(pos, vel, 5.0f, 1.0f));
	}
	for (int i = 0; i < MAXPARTICLES / 2; i++)
	{
		neg = -neg;
		float x = 5.0f + (i*particleSize);
		float y = 2.0f;
		glm::vec3 pos(x, y, 1);
		glm::vec3 vel(10.0f, 0.0f, 0);
		particles.push_back(Particle(pos, vel, 5.0f, 1.0f));
	}
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
		}
	}
}

void ParticleContainer::findNeighbors(Particle &p, int pIndex)
{
	for (int i = 0; i < MAXPARTICLES; i++)
	{
		if (i != pIndex)
		{
			Particle *j = &particles[i];
			if (glm::distance(p.pos, j->pos) < hVal)
			{
				p.addNeighbor(j);
			}
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
}


