/******************************************************************************/
/*!
\file		tile.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	TileMap contains a 2D array of Tiles, meant to represent the map.
			It also has helper functions to draw Tiles, get a Tile or set a
			Unit on a Tile.
*/
/******************************************************************************/
#pragma once
#include "unit.h"
#include "pathfinding.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>

enum TileType
{
	Grass, Forest, River, Mountain
};

class Tile : public sf::Transformable, public sf::Drawable
{
public:

	TileType m_tiletype;
	std::string m_tiletype_name;
	sf::Sprite m_sprite;
	sf::Vector2f m_pos;
	sf::Vector2i m_idx;

	float m_bonus;

	Unit * m_occupant = nullptr;
private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the entity's transform -- combine it with the one that was passed by the caller
		states.transform *= getTransform(); // getTransform() is defined by sf::Transformable
		// You can draw other high-level objects
		target.draw(m_sprite, states);
	}
};

class TileMap 
{
public:

	TileMap(const std::string& map);

	void GetTile(const sf::Vector2i & mousePos, Tile ** tile);
	void SetUnit(Unit*);
	float GetTileCost(const sf::Vector2i &);
	void RestartColor();
	void ShowAvailableMovement(std::unordered_map<int, Node*>);
	void ShowAvailableAttacks(Unit * attacker);
	void Clear();

	Tile** m_grid;
	int width, height;

};

class MasterTileMap 
{
public:
	void AddTileMap(TileMap tilemap)
	{
		m_tilemaps.push_back(tilemap);
	}

	void DrawCurrentTileMap(sf::RenderWindow & window);

	std::vector<TileMap> m_tilemaps;

	int m_current_tilemap = 0;


	Tile * selected_tile = nullptr;
	
};

