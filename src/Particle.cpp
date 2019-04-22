//find new m4 kernel

#include "glm\glm.hpp"
#include "Particle.h"
#define d 3 //3 dimensionns
#define p0 998.29f //from https://www10.cs.fau.de/publications/theses/2010/Staubach_BT_2010.pdf
#define k 3.0f //from https://www10.cs.fau.de/publications/theses/2010/Staubach_BT_2010.pdf table 3.2
#define vis 3.5f //from  https://www10.cs.fau.de/publications/theses/2010/Staubach_BT_2010.pdf
#define g glm::vec3(0.0f,-9.81f,0.0f)

//IHMSEN M., CORNELIS J., SOLENTHALER B., HORVATH C., TESCHNER M.: Implicit incompressible SPH.IEEE
//Transactions on Visualization and Computer Graphicss
//found in https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf
const float mj = pow(2.0f / 3.0f * hVal, 3)*p0;

Particle::Particle(glm::vec3 pos, glm::vec3 localVelocity, float life, float size)
	:pos(pos), localVelocity(localVelocity), life(life), size(size)
{
}


void Particle::CalcPressure()
{
	if (neighbors.size() > 0)
	{
		dPi = DensityPi();
		pressurePi = PressurePi(dPi);
	}
	else
	{
		dPi = { 1,1,1 };
		pressurePi = { 1,1,1 };
	}
}

glm::vec3 Particle::PressurePi(glm::vec3 pi)
{
	float x, y, z;
	x = pow(pi.x / p0, 7);
	y = pow(pi.y / p0, 7);
	z = pow(pi.z / p0, 7);
	glm::vec3 pressurePi(x, y, z);
	return k * pressurePi - 1.0f;
}

glm::vec3 Particle::DensityPi()
{

	//Algorithm 1 2014 SPH STAR
	glm::vec3 Pi = glm::vec3(0.0f, 0.0f, 0.0f);
	for (Particle *j : neighbors)
	{
		float q = glm::length((pos - j->pos)) / hVal;
		Pi += mj * W(q);
	}
	return Pi;
}

void Particle::CalcForces()
{
	Fi = fPressure() + fViscosity() + fOther();
}
//https://en.wikipedia.org/wiki/Smoothed-particle_hydrodynamics#Operators
//equation for pressure gradient
//also
//https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf
//equation 6 and algorithm 1
glm::vec3 Particle::fPressure()
{
	float sum = 0;
	glm::vec3 Pi = dPi;
	glm::vec3 Ai = pressurePi;
	glm::vec3 Pi2(pow(Pi.x, 2),pow(Pi.y,2),pow(Pi.z,2));
	glm::vec3 Pj;
	glm::vec3 Pj2;
	glm::vec3 a;
	float  b;
	glm::vec3 fPressure = { 0.0f,0.0f,0.0f };
	for (Particle *p : neighbors)
	{
		glm::vec3 Aj = p->pressurePi;
		 Pj = p->dPi;
		Pj2 = glm::vec3(pow(Pj.x, 2), pow(Pj.y, 2), pow(Pj.z, 2));

		a = mj * (Ai / Pi2 + Aj / Pj2);
		b = Wgradient((Ai - Aj).length() / hVal);
		fPressure += a * b;
	}
	fPressure *= -dPi;
	//pointless division by pressurePi
	fPressure = -mj / dPi * fPressure;
	return fPressure;
}

glm::vec3 Particle::fViscosity()
{
	glm::vec3 sum(0.0f, 0.0f, 0.0f);
	glm::vec3 vi = localVelocity;
	for (Particle *p : neighbors)
	{
		glm::vec3 pj = p->dPi;
		glm::vec3 vj = p->localVelocity;
		glm::vec3 vij = vi - vj;
		glm::vec3 xij = (pos - p->pos);
		sum += mj / pj * vij * 
		(xij * Wgradient((vi-vj).length()/ hVal)) /
			(glm::dot(xij, xij) + (0.01f*pow(hVal, 2)));

	}
	sum *= 2;
	return mj * vis * sum;
}

glm::vec3 Particle::fOther()
{
	return p0* g;
}

//glm::vec3 Particle::W(glm::vec3 v)
//{
//	return glm::vec3(
//		W(v.x),
//		W(v.y),
//		W(v.z));
//}

float Particle::W(float q)
{
	float fq;
	if (0 <= q && q < 1)
	{
		fq = 2.0f / 3.0f - pow(q, 2) + 1.0f / 2.0f*pow(q, 3);
		fq *= 3.0f / 2.0f*M_PI;
	}
	else if (1 <= q && q < 2)
	{
		fq = 1.0f / 6.0f*pow(2.0f - q, 3);
		fq *= 3.0f / 2.0f*M_PI;

	}
	else
	{
		fq = 0;
	}
	float wq = (1.0f / pow(hVal, d))*fq;
	return wq;
}

glm::vec3 Particle::Wgradient(glm::vec3 v)
{
	return glm::vec3(
		Wgradient(abs(v.x)),
		Wgradient(abs(v.y)),
		Wgradient(abs(v.z)));
}
//I am just taking the derivative or maybe partial derivative of W (m4 cubic spline) i am not sure if this is correct
//derived from https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf eq 5
float Particle::Wgradient(float q)
{
	float fq;
	if (0 < q && q < 1)
	{
		fq = (-2 * q) + (3.0f*pow(q, 2)/2.0f);
	}
	else if (1 <= q && q < 2)
	{	
		fq = 1.0f / 6.0f * pow(3 * (2 - q), 2)*(-1);
	}
	else
	{
		return 0;
	}
	float wq = (1.0f / pow(hVal, d))*3.0f / (2.0f*M_PI)*fq;
	return wq;
}




void Particle::CalcVelocity(float dt)
{
		Fi.z = 0;
		localVelocity += dt * (Fi) / mj;
}

void Particle::CalcPosition(float dt)
{
	pos += dt * localVelocity;

	if (pos.y < -20.0f)
	{
		localVelocity.y = -localVelocity.y/2;
		pos.y = -20.0f;
	}
	}



Particle::~Particle()
{
}

void Particle::clear()
{
	neighbors.clear();
	Fi = glm::vec3( 0,0,0);
}
void Particle::addNeighbor(Particle *p)
{
	neighbors.push_back(p);
}
