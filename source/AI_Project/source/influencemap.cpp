/******************************************************************************/
/*!
\file		influencemap.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Influence Maps are defined here (Tile, Ally, Enemy). It's also
			updated wherever is needed.
*/
/******************************************************************************/

#include "global.h"
#include "influencemap.h"
#include "tile.h"

void InfluenceMap::CreateInitialIM()
{
	//Memory is already allocated
	if (m_tile_bonus_IM != nullptr)
		return;

	m_tile_bonus_IM = new float*[g_current_grid.height];
	m_ally_IM = new float*[g_current_grid.height];
	m_enemy_IM = new float*[g_current_grid.height];

	for (int i = 0; i < g_current_grid.height; i++)
	{
		m_tile_bonus_IM[i] = new float[g_current_grid.width];
		m_ally_IM[i] = new float[g_current_grid.width];
		m_enemy_IM[i] = new float[g_current_grid.width];
	}

	for(int i = 0; i < g_current_grid.height; i++)
		for (int j = 0; j < g_current_grid.width; j++)
		{
			switch (g_current_grid.m_grid[i][j].m_tiletype)
			{
			case Mountain:
			{
				m_tile_bonus_IM[i][j] = 1.f;
				break;
			}
			case Forest:
			{
				m_tile_bonus_IM[i][j] = 0.5f;
				break;
			}
			case Grass:
			{
				m_tile_bonus_IM[i][j] = 0.0f;
				break;
			}
			case River:
			{
				m_tile_bonus_IM[i][j] = -1.0f;
				break;
			}
			default:
				break;
			}
		}


	UpdateAllyIM();
	UpdateEnemyIM();
}

void InfluenceMap::UpdateAllyIM()
{
	for (int i = 0; i < g_current_grid.height; i++)
		for (int j = 0; j < g_current_grid.width; j++)
		{
			m_ally_IM[i][j] = 0;
		}
	
	for (auto it : g_unit_manager.m_enemy_units)
	{
		auto nodes = g_Dijkstra.FindAllPaths(it->m_current_tile->m_idx, it->m_tile_movement, Player);

		for (auto ir : nodes)
		{
			m_ally_IM[ir.second->m_cell.x][ir.second->m_cell.y] += 1.f / static_cast<float>(g_unit_manager.m_enemy_units.size());
		}
	}
}

void InfluenceMap::UpdateEnemyIM()
{
	for (int i = 0; i < g_current_grid.height; i++)
		for (int j = 0; j < g_current_grid.width; j++)
		{
			m_enemy_IM[i][j] = 0;
		}

	for (auto it : g_unit_manager.m_player_units)
	{
		auto nodes = g_Dijkstra.FindAllAttackbleTiles(it->m_current_tile->m_idx, it->m_tile_movement, Enemy);

		for (auto ir : nodes)
		{
			m_enemy_IM[ir.second->m_cell.x][ir.second->m_cell.y] += 1.f / static_cast<float>(g_unit_manager.m_player_units.size());
		}
	}
}

void InfluenceMap::Clear()
{
	if (m_tile_bonus_IM == nullptr)
		return;

	for (int i = 0; i < g_current_grid.height; i++)
	{
		delete[] m_tile_bonus_IM[i];
		delete[] m_ally_IM[i];
		delete[] m_enemy_IM[i];
	}

	delete[] m_tile_bonus_IM;
	delete[] m_ally_IM;
	delete[] m_enemy_IM;


	m_tile_bonus_IM	= nullptr;
	m_ally_IM = nullptr;
	m_enemy_IM = nullptr;
}
