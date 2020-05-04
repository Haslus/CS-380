/******************************************************************************/
/*!
\file		pathfinding.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Dijkstra algorithm is implemented here. It's used to find all the
			Tiles an Unit can go to, and also all attackable Tiles.
*/
/******************************************************************************/
#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <list>

#include "unit.h"

class Node
{
public:
	Node() = default;
	Node(sf::Vector2i cell, float G = 0, float F = 0, Node * parent = nullptr) : m_G(G), m_F(F), m_cell(cell), m_parent(parent) {};

	sf::Vector2i m_cell;
	float m_G; //G(X)
	float m_F; //G(X) + H(X)

	Node * m_parent;

	bool operator==(const Node& rhs) const
	{
		return m_cell == rhs.m_cell;
	}
};


class Dijkstra
{
public:
	void Clear();
	std::unordered_map<int, Node*> FindAllPaths(sf::Vector2i source, int movement, OwnerType wall);
	std::unordered_map<int, Node*> FindAllAttackbleTiles(sf::Vector2i source, int movement, OwnerType wall);

	std::unordered_map<int, Node*> m_path;

private:
	bool ValidMovement(sf::Vector2i current, sf::Vector2i direction, OwnerType wall);
	Node* FindNode(sf::Vector2i next_cell);

	sf::Vector2i directions[8] = { {0,1},{ 1,0 },{ 0,-1 },{ -1,0 },
								{ 1,1 },{ 1,-1 },{ -1,-1 },{ -1,1 } };

	std::list<Node*> openList;
	std::unordered_map<int, Node*> closedList;
	Node * current = nullptr;

};
