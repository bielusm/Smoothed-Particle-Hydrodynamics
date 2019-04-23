#pragma once
#include "Particle.h"
#include <vector>
class Grid
{
	class Indices
	{
	public:
		std::vector<int> index;

	};
public:
	Grid() = default;
	Grid(float left, float right, float top, float bottom, float front, float back);
	~Grid();
	glm::ivec3 Grid::updateCoord(int index, glm::ivec3 gridCoords, glm::vec3 newPos);
	glm::ivec3 Grid::normalizeCoords(glm::vec3 pos);
	void makeGrid(std::vector<Particle>& particles);
	bool inBounds(glm::vec3);
	std::vector<int> neighborIndices(Particle p);

private: 
	Indices ***grid;

	

	float left, right, top, bottom, front, back;
	int width, height, depth;
};

