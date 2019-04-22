#pragma once
#include <glm\common.hpp>
#include <vector>
#define particleSize 0.3f
#define hVal 3.0f*particleSize
#define _USE_MATH_DEFINES
#include <math.h>
class Particle
{
public:
	Particle() = default;
	Particle(glm::vec3 pos, glm::vec3 localVelocity, float life, float size);
	~Particle();

	float W(float q);
	float Wgradient(float q);

	glm::vec3 PressurePi(glm::vec3 pi);
	glm::vec3 Particle::DensityPi();
	void clear();
	void addNeighbor(Particle *p);
	void CalcForces();

	glm::vec3 Particle::fPressure();
	glm::vec3 fViscosity();
	glm::vec3 Particle::fOther();
	glm::vec3 Wgradient(glm::vec3);
	

	
	void Particle::CalcVelocity(float dt);
	void Particle::CalcPosition(float dt);
	
	glm::vec3 localVelocity;

	void CalcPressure();
	glm::vec3 dPi;
	std::vector<Particle*> neighbors;
	glm::vec3 pos;
	float size;
	float life;
	glm::vec3 Fi;
	glm::vec3 pressurePi;
};

