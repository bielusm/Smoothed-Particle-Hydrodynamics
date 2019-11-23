#include "Grid.h"
#include <iostream>

//from https://stackoverflow.com/questions/10238699/dynamically-allocating-3d-array
Grid::Grid(float left, float right, float top, float bottom)
	:left(left), right(right), top(top), bottom(bottom)
{
	width = (int)(1 + abs(right - left) / hVal);
	height = (int)(1 + abs(top - bottom) / hVal);
	std::cout << "Making grid\n";
	grid = new Indices *[width];
	for (int i = 0; i < width; i++)
	{
		grid[i] = new Indices[height];
	}

}

Grid::~Grid()
{
	for (int i = 0; i < width; i++)
	{
		delete[] grid[i];
	}
	delete[] grid;
}

glm::ivec2 Grid::normalizeCoords(glm::vec2 pos)
{

	float xRatio = (left - right)/width;
	if (pos.x < left || pos.x > right || pos.y < top || pos.y > bottom)
		return glm::vec2(-1,-1);
	float size = left + right;
	int x = (int)((pos.x +right/hVal));
	int y = (int)((pos.y+bottom) / hVal);
	return glm::ivec2(x, y);
}

glm::ivec2 Grid::updateCoord(int index, glm::ivec2 gc, glm::vec2 newPos)
{
	glm::ivec2 pos = normalizeCoords(newPos);
	if (glm::all(glm::equal(gc, pos))|| gc.x <0)
		return gc;
	else
	{
		bool found = false;

		std::vector<int> &indices = grid[gc.x][gc.y].index;
		for (unsigned int i = 0; i < indices.size(); i++)
		{
			if (indices[i] == index)
			{
				indices.erase(indices.begin() + i);
				found = true;
				break;
			}
		}
		if (!found)
		{
			std::cout <<"not found";
		}
		if (pos.x < 0)
			return glm::ivec3(-1, -1, -1);
		grid[pos.x][pos.y].index.push_back(index);
		return pos;	
	}


}

void Grid::makeGrid(std::vector<Particle>& particles)
{
	for (Particle & p : particles)
	{
		glm::ivec2 pos = normalizeCoords(p.pos);
		if (pos.x > -1)
		{
			grid[pos.x][pos.y].index.push_back(p.index);
			p.gridCoords = pos;
		}
		else
		{
			p.gridCoords = glm::ivec3(-1.0f, -1.0f, -1.0f);
		}
	}
}
bool Grid::inBounds(glm::vec2 p )
{
	return (p.x > 0 && p.x < width && p.y > 0 && p.y < height);
}

std::vector<int> Grid::neighborIndices(Particle p)
{
	std::vector<int>neighbors;
	glm::ivec2 gc = p.gridCoords;
	if (gc.x < 0)
		return neighbors;
	else
	{
		for (int x = gc.x-1; x < gc.x+1; x++)
			for(int y = gc.y-1; y < gc.y+1;y++)
				{
					if (inBounds(glm::vec2(x, y)))
					{
						std::vector <int> index = grid[x][y].index;
						for (unsigned int i = 0; i < index.size(); i++)
						{
							if (index[i] != p.index)
								neighbors.push_back(index[i]);
						}
					}
				}
		return neighbors;
	}

		 
}
