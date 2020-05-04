/******************************************************************************/
/*!
\file		pathfinding.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Dijkstra algorithm is implemented here. It's used to find all the
			Tiles an Unit can go to, and also all attackable Tiles.
*/
/******************************************************************************/
#include "global.h"
#include "pathfinding.h"
#include "tile.h"


std::unordered_map<int, Node*>  Dijkstra::FindAllPaths(sf::Vector2i source, int movement, OwnerType wall)
{
	
	if (current == nullptr)
	{
		Node * start = new Node(source);
		//Push start node onto open list
		openList.push_back(start);
	}
	else
	{
		Clear();
		Node * start = new Node(source);
		//Push start node onto open list
		openList.push_back(start);
	}

	//While open list is not empty
	while (openList.empty() == false)
	{
		//Get the node with the lowest F value
		current = openList.front();
		openList.remove(current);

		//Check adjacent nodes
		for (unsigned i = 0; i < 4; i++)
		{
			sf::Vector2i next_cell = current->m_cell + directions[i];

			//Check if the new position is valid
			if (ValidMovement(current->m_cell, directions[i], wall) == false)
				continue;

			float g_x = current->m_G + g_current_grid.GetTileCost(next_cell);
	
			float f_x = g_x;

			if (g_x > movement)
			{
				continue;
			}

			//If it's on the closed list, we stop
			auto it = closedList.find(next_cell.x * g_current_grid.width + next_cell.y);
			if (it != closedList.end())
				continue;

			//Search on the openList
			Node* existing_child = FindNode(next_cell);
			//If it isnt on the openlist, create
			if (existing_child == nullptr)
			{
				Node* child = new Node(next_cell, g_x, f_x, current);
				openList.push_back(child);
			}
			//Else if its smaller cost, replace
			else if (existing_child->m_F > f_x)
			{
				openList.remove(existing_child);

				Node* child = new Node(next_cell, g_x, f_x, current);
				openList.push_back(child);
			}
		}

		closedList[current->m_cell.x * g_current_grid.height + current->m_cell.y] = current;
	}

	m_path = closedList;

	return m_path;
}

std::unordered_map<int, Node*> Dijkstra::FindAllAttackbleTiles(sf::Vector2i source, int movement, OwnerType wall)
{

	auto nodes = FindAllPaths(source, movement, wall);
	auto temp = nodes;

	for (auto ir : temp)
	{
		for (int i = 0; i < 4; i++)
		{
			sf::Vector2i attacking_tile = ir.second->m_cell + directions[i];

			//If it's an invalid move skip
			if (attacking_tile.x < 0 || attacking_tile.y < 0 ||
				attacking_tile.x >= g_current_grid.height || attacking_tile.y >= g_current_grid.width)
			{
				continue;
			}

			nodes[attacking_tile.x * g_current_grid.height + attacking_tile.y] = new Node(attacking_tile, 0, 0, 0);
		}
	}

	return nodes;
}

bool Dijkstra::ValidMovement(sf::Vector2i current, sf::Vector2i direction, OwnerType wall)
{
	sf::Vector2i next_cell = current + direction;

	if (g_current_grid.height <= next_cell.x || g_current_grid.width <= next_cell.y
		|| next_cell.x < 0 || next_cell.y < 0)
		return false;

	if (g_current_grid.m_grid[next_cell.x][next_cell.y].m_occupant != nullptr && g_current_grid.m_grid[next_cell.x][next_cell.y].m_occupant->m_owner == wall)
		return false;

	

	else
		return true;

}

/**
* @brief 	Frees the memory of the nodes
* @return	Parsed file
*/
void Dijkstra::Clear()
{
	if(!openList.empty())
		for (auto it = openList.begin(); it != openList.end(); ++it)
			delete *it;

	if (!closedList.empty())
		for (auto it = closedList.begin(); it != closedList.end(); ++it)
			delete it->second;

	openList.clear();
	closedList.clear();

	current = nullptr;
}

/**
* @brief 	Search the given node by pos
* @param	next_cell
* @return	Return the node, if found
*/
Node* Dijkstra::FindNode(sf::Vector2i next_cell)
{
	for (auto it = openList.begin(); it != openList.end(); ++it)
		if ((*it)->m_cell == next_cell)
			return *it;

	return nullptr;

}
