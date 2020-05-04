/******************************************************************************/
/*!
\file		tile.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	TileMap contains a 2D array of Tiles, meant to represent the map.
			It also has helper functions to draw Tiles, get a Tile or set a
			Unit on a Tile.
*/
/******************************************************************************/
#include "global.h"
#include "tile.h"
#include "pathfinding.h"

TileMap::TileMap(const std::string & map)
{
	std::vector<std::string> tiles;
	std::ifstream file(map);

	if (!file)
	{
		std::cout << "error opening tilemap" << std::endl;
		return;
	}

	while (!file.eof())
	{
		std::string temp;
		std::getline(file, temp);
		tiles.push_back(temp);
	}
	width = static_cast<int>(tiles[0].size());
	height = static_cast<int>(tiles.size());

	//Allocate memory for the grid

	m_grid = new Tile*[height];

	for (int i = 0; i < height; ++i)
		m_grid[i] = new Tile[width];

	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			// get the current tile number
			int tileNumber;

			switch (tiles[i][j])
			{
			case 'M':
			{
				m_grid[i][j].m_tiletype = Mountain;
				m_grid[i][j].m_sprite.setTexture(g_textures.GetTexture("Mountain"));
				m_grid[i][j].m_tiletype_name = "Mountain";
				tileNumber = 3;
				m_grid[i][j].m_bonus = 2;
				break;
			}
			case 'F':
			{
				m_grid[i][j].m_tiletype = Forest;
				m_grid[i][j].m_sprite.setTexture(g_textures.GetTexture("Forest"));
				m_grid[i][j].m_tiletype_name = "Forest";
				tileNumber = 2;
				m_grid[i][j].m_bonus = 1;
				break;
			}
			case 'R':
			{
				m_grid[i][j].m_tiletype = River;
				m_grid[i][j].m_sprite.setTexture(g_textures.GetTexture("River"));
				m_grid[i][j].m_tiletype_name = "River";
				tileNumber = 1;
				m_grid[i][j].m_bonus = -1;
				break;
			}
			case 'G':
			{
				m_grid[i][j].m_tiletype = Grass;
				m_grid[i][j].m_sprite.setTexture(g_textures.GetTexture("Grass"));
				m_grid[i][j].m_tiletype_name = "Grass";
				tileNumber = 0;
				m_grid[i][j].m_bonus = 0;
				break;
			}
			default:
			{
				m_grid[j][i].m_tiletype = Grass;
				m_grid[i][j].m_sprite.setTexture(g_textures.GetTexture("Grass"));
				m_grid[i][j].m_tiletype_name = "Grass";
				tileNumber = 0;
				break;
			}
			}

			sf::Vector2f p0 = sf::Vector2f(static_cast<float>(j * 64), static_cast<float>(i * 64));
			sf::Vector2f p1 = sf::Vector2f(static_cast<float>(j + 1) * 64.f, static_cast<float>(i + 1) * 64.f);
			m_grid[i][j].m_sprite.setOrigin(32, 32);
			m_grid[i][j].m_pos = (p1 - p0) / 2.f + p0;
			m_grid[i][j].setPosition(m_grid[i][j].m_pos);

			m_grid[i][j].m_idx = sf::Vector2i{ i,j };
		}

}

void TileMap::GetTile(const sf::Vector2i & mousePos, Tile ** tile)
{
	if (mousePos.x == -1)
	{
		*tile = nullptr;
		return;
	}


	int idx_X = mousePos.y / 64;
	int idx_Y = mousePos.x / 64;

	if (idx_X < 0 || idx_X >= height ||
		idx_Y < 0 || idx_Y >= width)
	{
		*tile = nullptr;
		return;
	}

	*tile = &m_grid[idx_X][idx_Y];


}

void TileMap::SetUnit(Unit * unit)
{
	sf::Vector2i tile_idx = { (static_cast<int>(unit->getPosition().y) - 32) / 64, (static_cast<int>(unit->getPosition().x) - 32) / 64 };

	if (m_grid[tile_idx.x][tile_idx.y].m_occupant != nullptr)
	{
		for (unsigned i = 0; i < g_unit_manager.m_all_units.size(); i++)
		{
			if (g_unit_manager.m_all_units[i] == m_grid[tile_idx.x][tile_idx.y].m_occupant)
			{
				g_unit_manager.m_all_units.erase(g_unit_manager.m_all_units.begin() + i);
				delete m_grid[tile_idx.x][tile_idx.y].m_occupant;
			}
		}
			
	}

	m_grid[tile_idx.x][tile_idx.y].m_occupant = unit;
	unit->m_current_tile = &m_grid[tile_idx.x][tile_idx.y];
}

float TileMap::GetTileCost(const sf::Vector2i & cell)
{

	TileType tile_type = m_grid[cell.x][cell.y].m_tiletype;

	switch (tile_type)
	{
	case Grass:
		return 1.f;
	case Forest:
		return 1.f;
	case River:
		return 2.f;
	case Mountain:
		return 2.f;
	default:
		return 0;
	}
}

void TileMap::RestartColor()
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			m_grid[i][j].m_sprite.setColor(sf::Color(255, 255, 255, 255));
		}
}

void TileMap::ShowAvailableMovement(std::unordered_map<int, Node*> tiles)
{
	for (auto it : tiles)
		m_grid[it.second->m_cell.x][it.second->m_cell.y].m_sprite.setColor(sf::Color(100,100,255,255));
}

void TileMap::ShowAvailableAttacks(Unit * attacker)
{
	for (auto it : g_unit_manager.m_enemy_units)
	{
		sf::Vector2i vec = { it->m_current_tile->m_idx - attacker->m_current_tile->m_idx };

		int distance = std::abs(vec.x) + std::abs(vec.y);

		if (distance == 1)
		{
			it->m_current_tile->m_sprite.setColor(sf::Color(255, 100, 100, 255));
		}

		
	}
}

void TileMap::Clear()
{
	if (m_grid == nullptr)
		return;

	for (int i = 0; i < height; ++i)
		delete [] m_grid[i];

	delete [] m_grid;

	m_grid = nullptr;
}

void MasterTileMap::DrawCurrentTileMap(sf::RenderWindow & window)
{
	for (int i = 0; i < g_grid.m_tilemaps[g_grid.m_current_tilemap].height; i++)
		for (int j = 0; j < g_grid.m_tilemaps[g_grid.m_current_tilemap].width; j++)
		{
			window.draw(g_current_grid.m_grid[i][j]);

		}
}
