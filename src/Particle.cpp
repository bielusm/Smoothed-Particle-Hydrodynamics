//find new m4 kernel
#include <iostream>
#include "glm\glm.hpp"
#include "Particle.h"
#define d 2 // dimensionns
#define p0 998.29f //from https://www10.cs.fau.de/publications/theses/2010/Staubach_BT_2010.pdf
#define k 3.0f //from https://www10.cs.fau.de/publications/theses/2010/Staubach_BT_2010.pdf table 3.2
#define vis 3.5f //from  https://www10.cs.fau.de/publications/theses/2010/Staubach_BT_2010.pdf
#define g glm::vec2(0.0f,-9.81f)
#define absorbtion 10.0f

//IHMSEN M., CORNELIS J., SOLENTHALER B., HORVATH C., TESCHNER M.: Implicit incompressible SPH.IEEE
//Transactions on Visualization and Computer Graphicss
//found in https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf
const float mj = pow(2.0f / 3.0f * hVal, 3)*p0;

Particle::Particle(glm::vec2 pos, glm::vec2 localVelocity, int index, float size)
	:pos(pos), localVelocity(localVelocity), index(index)
{
}

void Particle::CalcPressure()
{
	if (neighbors.size() > 0)
	{
		//dPi = DensityPi();
		pressurePi = PressurePi(idPi);
	}
	else
	{
		idPi = glm::vec2(p0, p0);
		pressurePi = PressurePi(idPi);
	}
}

glm::vec2 Particle::PressurePi(glm::vec2 dPi)
{
	float x, y;
	x = pow(idPi.x / p0, 7);
	y = pow(idPi.y / p0, 7);
	glm::vec2 pressurePi(x, y);
	return k * (pressurePi - 1.0f);
}

//glm::vec2 Particle::DensityPi()
//{
//
//	//Algorithm 1 2014 SPH STAR
//	glm::vec3 Pi = glm::vec3(0.0f, 0.0f, 0.0f);
//	for (Particle *j : neighbors)
//	{
//		float q = glm::length((pos - j->pos)) / hVal;
//		Pi += mj * poly6(q);
//	}
//	return Pi;
//}

//void Particle::CalcForces()
//{
//	Fi = fPressure() + fViscosity() + fOther();
//}

void Particle::CalcImmediateVelocity(float dt)
{
	glm::vec2 fVisG = fViscosity() + fOther();
	immediateVel = localVelocity + dt * (fVisG / mj);
	
}

void Particle::CalcImmediateDensity(float dt)
{

	glm::vec2 sum(0, 0);
	float a = 0;
	glm::vec2 b(0, 0);
	for (Particle *p : neighbors)
	{
		float xij = glm::length(pos - p->pos);


		a += mj * poly6(abs(xij));

	
	}
	for (Particle *p : neighbors)
	{
		float xij = glm::length(pos - p->pos);
		b += (immediateVel - p->immediateVel)*Wgradient((xij/hVal));
	}
	sum = a + dt * b;
	idPi = sum;
}

//https://en.wikipedia.org/wiki/Smoothed-particle_hydrodynamics#Operators
//equation for pressure gradient
//also
//https://cg.informatik.uni-freiburg.de/publications/2014_EG_SPH_STAR.pdf
//equation 6 and algorithm 1
//now https://www10.cs.fau.de/publications/theses/2010/Staubach_BT_2010.pdf
static int counter = 0;
void Particle::fPressure()
{

	float sum = 0;
	glm::vec2 Pi = idPi;
	glm::vec2 Ai = pressurePi;
	glm::vec2 Pi2(pow(Pi.x, 2),pow(Pi.y,2));
	glm::vec2 Pj;
	glm::vec2 Pj2;
	glm::vec2 a;
	float  b;
	pressureForce = { 0.0f,0.0f };
	for (Particle *p : neighbors)
	{
		glm::vec2 Aj = p->pressurePi;
		 Pj = p->idPi;
		Pj2 = glm::vec2(pow(Pj.x, 2), pow(Pj.y, 2));

		a =  (Ai / Pi2 + Aj / Pj2)*mj;
		b = spikyGrad((glm::length(pos-p->pos)));
		pressureForce += a * b;
		if (index == 33)
		{
			if (counter == 28)
				std::cout << "break";
			std::cout << counter;
			counter++;
		}
	}
	pressureForce *= -idPi;

	//fPressure = -mj / dPi * fPressure;
}

glm::vec2 Particle::fViscosity()
{
	glm::vec2 sum(0.0f, 0.0f);
	glm::vec2 vi = localVelocity;
	for (Particle *p : neighbors)
	{
		glm::vec2 pj = p->idPi;
		glm::vec2 vj = p->localVelocity;
		glm::vec2 vij = vi - vj;
		glm::vec2 xij = (pos - p->pos);
		sum += mj / p0 * vij * 
	/*	(xij * spikyGrad(abs(glm::length(xij)))) /
			(glm::dot(xij, xij) + (0.01f*pow(hVal, 2)));*/
		sum += vij * mj / p0 * viscosityKernel(glm::length(xij));
	}
	return vis *mj* 2 * sum;
}

glm::vec2 Particle::fOther()
{
	return p0* g;
}

float Particle::poly6(float r)
{
	if (r > hVal || r < 0)
		return 0;
	float kr = (315.0f)
		/ (64.0f * M_PI*pow(hVal, 9));
	kr *= pow(pow(hVal, 2) - pow(r, 2), 3);
	return kr;
}

float Particle::viscosityLap(float r)
{
	if (abs(r) > hVal)
		return 0;
	return 45.0f / (M_PI*pow(hVal, 6))*(hVal - abs(r));
}

float Particle::spikyGrad(float r)
{
	if (abs(r) > hVal)
		return 0;
	return -45.0f / (M_PI*pow(hVal, 6))*pow((hVal - r), 2);

}
float Particle::W(float q)
{
	float fq;
	if (0 <= q && q < 1)
	{
		fq = 2.0f / 3.0f - pow(q, 2) + 1.0f / 2.0f*pow(q, 3);
		fq *= 10.0f / 7.0f*M_PI;
	}
	else if (1 <= q && q < 2)
	{
		fq = 1.0f / 6.0f*pow(2.0f - q, 3);
		fq *= 10.0f / 7.0f*M_PI;
	}
	else
	{
		fq = 0;
	}
	float wq = (1.0f / pow(hVal, d))*fq;
	return wq;
}

float Particle::WLaplacian(float q)
{
	float fq;
	if (0 < q && q < 1)
	{
		fq = 3 * q - 2;
	}
	else if (1 <= q && q < 2)
	{
		fq = 3 * (2 - q);
	}
	else
	{
		return 0;
	}
	float wq = (1.0f / pow(hVal, d))*10.0f / (7.0f*M_PI)*fq;
	return wq;
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
	float wq = (1.0f / pow(hVal, d))*10.0f / (7.0f*M_PI)*fq;
	return wq;
}




void Particle::CalcVelocity(float dt)
{
		localVelocity = immediateVel +  dt * (pressureForce) / mj;
}

void Particle::CalcPosition(float dt)
{
	pos += dt * localVelocity;
	//if (pos.x < -10.0f)
	//{
	//	pos.x = -10.0f;
	//	localVelocity.x = -localVelocity.x / absorbtion;
	//}
	//else if (pos.x > 10.0f)
	//{
	//	pos.x = 10.0f;
	//	localVelocity.x = -localVelocity.x / absorbtion;
	//}
	//if (pos.y < -10.0f)
	//{
	//	pos.y = -10.0f;
	//	localVelocity.y = -localVelocity.x / absorbtion;
	//}
	//if (pos.y > 20.0f)
	//{
	//	pos.y = 20.0f;
	//	localVelocity.y = -localVelocity.x / absorbtion;
	//}
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
