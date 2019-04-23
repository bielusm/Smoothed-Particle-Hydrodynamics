#include "Grid.h"
#include <iostream>



//from https://stackoverflow.com/questions/10238699/dynamically-allocating-3d-array
Grid::Grid(float left, float right, float top, float bottom, float front, float back)
	:left(left), right(right), top(top), bottom(bottom), front(front), back(back)
{
	 width = (int)(1 + abs(right - left) / hVal);
	 height = (int)(1 + abs(top - bottom) / hVal);
	 depth = (int)(1 + abs(front - back) / hVal);
	 std::cout << "Making grid\n";
	grid = new Indices **[width];
	for (int i =0; i < width; i++)
	{
		grid[i] = new Indices*[height];
		for (int j = 0; j < height; j++)
		{
			grid[i][j] = new Indices[depth];
		}
	}
	
}

Grid::~Grid()
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
	//		delete[] grid[i][j];
		}
		delete[] grid[i];
	}
	delete[] grid;
}

glm::ivec3 Grid::normalizeCoords(glm::vec3 pos)
{

	float xRatio = (left - right)/width;
	if (pos.x < left || pos.x > right || pos.y < top || pos.y > bottom || pos.z < front || pos.z > back)
		return glm::vec3(-1,-1,-1);
	float size = left + right;
	int x = (int)((pos.x +right/hVal));
	int y = (int)((pos.y+bottom) / hVal);
	int z = (int)((pos.z+back) / hVal);
	return glm::ivec3(x, y, z);
}

glm::ivec3 Grid::updateCoord(int index, glm::ivec3 gc, glm::vec3 newPos)
{
	glm::ivec3 pos = normalizeCoords(newPos);
	if (glm::all(glm::equal(gc, pos))|| gc.x <0)
		return gc;
	else
	{
		bool found = false;

		std::vector<int> &indices = grid[gc.x][gc.y][gc.z].index;
		for (int i = 0; i < indices.size(); i++)
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
		grid[pos.x][pos.y][pos.z].index.push_back(index);
		return pos;	
	}


}

void Grid::makeGrid(std::vector<Particle>& particles)
{
	for (Particle & p : particles)
	{
		glm::ivec3 pos = normalizeCoords(p.pos);
		if (pos.x > -1)
		{
			grid[pos.x][pos.y][pos.z].index.push_back(p.index);
			p.gridCoords = pos;
		}
		else
		{
			p.gridCoords = glm::ivec3(-1.0f, -1.0f, -1.0f);
		}
	}
}
bool Grid::inBounds(glm::vec3 p )
{
	return (p.x > 0 && p.x < width && p.y > 0 && p.y < height && p.z > 0 && p.z < depth);
}

std::vector<int> Grid::neighborIndices(Particle p)
{
	std::vector<int>neighbors;
	glm::ivec3 gc = p.gridCoords;
	if (gc.x < 0)
		return neighbors;
	else
	{
		for (int x = gc.x-1; x < gc.x+1; x++)
			for(int y = gc.y-1; y < gc.y+1;y++)
				for (int z = gc.z-1; z < gc.z+1; z++)
				{
					if (inBounds(glm::vec3(x, y, z)))
					{
						std::vector <int> index = grid[x][y][z].index;
						for (int i = 0; i < index.size(); i++)
						{
							if (index[i] != p.index)
								neighbors.push_back(index[i]);
						}
					}
				}
		return neighbors;
	}

		 
}
