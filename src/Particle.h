#pragma once
#include <glm\common.hpp>
#include <vector>
#define particleSize 0.2f
#define _USE_MATH_DEFINES
#include <math.h>

static float hVal = 3.0f*particleSize;
class Particle
{
public:
	Particle() = default;
	Particle(glm::vec2 pos, glm::vec2 localVelocity, int index, float size);
	~Particle();

	float W(float q);
	float Wgradient(float q);
	float WLaplacian(float q);
	
	glm::vec2 PressurePi(glm::vec2 pi);
	glm::vec2 Particle::DensityPi();
	void clear();
	void addNeighbor(Particle *p);
	void CalcForces();

	void CalcImmediateVelocity(float dt);

	void CalcImmediateDensity(float dt);

	void Particle::fPressure();
	glm::vec2 fViscosity();
	glm::vec2 Particle::fOther();
	

	
	void Particle::CalcVelocity(float dt);
	void Particle::CalcPosition(float dt);
	
	glm::vec2 localVelocity;
	glm::vec2 immediateVel;
	void CalcPressure();
	glm::vec2 idPi;
	glm::vec2 dPi;
	std::vector<Particle*> neighbors;
	glm::vec2 pos;
	float size;
	glm::vec2 Fi;
	glm::vec2 pressureForce;
	glm::vec2 pressurePi;
	glm::ivec2 gridCoords;
	int index;




};

