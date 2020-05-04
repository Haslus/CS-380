/******************************************************************************/
/*!
\file		influencemap.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Influence Maps are defined here (Tile, Ally, Enemy). It's also
			updated wherever is needed.
*/
/******************************************************************************/
#pragma once

class InfluenceMap
{
public:
	void CreateInitialIM();
	void UpdateAllyIM();
	void UpdateEnemyIM();
	void Clear();

	float** m_tile_bonus_IM = nullptr;
	float** m_ally_IM = nullptr;
	float** m_enemy_IM = nullptr;

	
};