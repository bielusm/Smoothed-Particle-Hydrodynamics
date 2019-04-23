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
	Grid(float left, float right, float top, float bottom);
	~Grid();
	glm::ivec2 Grid::updateCoord(int index, glm::ivec2 gc, glm::vec2 newPos);
	glm::ivec2 Grid::normalizeCoords(glm::vec2 pos);
	void makeGrid(std::vector<Particle>& particles);
	std::vector<int> neighborIndices(Particle p);
	bool Grid::inBounds(glm::vec2 p);
private: 
	Indices **grid;

	float left, right, top, bottom;
	int width, height;
};

