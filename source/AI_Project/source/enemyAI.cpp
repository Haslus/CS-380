/******************************************************************************/
/*!
\file		enemyAI.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Enemy AI is defined here, the Unit used for the AI. Strategic AI is
			also defined here.
*/
/******************************************************************************/
#include "global.h"
#include "enemyAI.h"
#include "pathfinding.h"
#include "tile.h"
#include "influencemap.h"


EnemyAI::EnemyAI(UnitType ut, OwnerType ot, sf::Vector2i pos)
{
	switch (ut)
	{
	case Saber:
	{
		m_unittype = ut;
		m_owner = ot;
		m_max_hp = 12;
		m_hp = 12;
		m_weapon_damage = 6;
		m_armor_defense = 3;
		m_tile_movement = 3;

		m_sprite.setTexture(g_textures.GetTexture("Saber_enemy"));
		m_owner_name = "Enemy";
		break;
	}
	case Lancer:
	{
		m_unittype = ut;
		m_owner = ot;
		m_max_hp = 15;
		m_hp = 15;
		m_weapon_damage = 5;
		m_armor_defense = 4;
		m_tile_movement = 2;

		m_sprite.setTexture(g_textures.GetTexture("Lancer_enemy"));
		m_owner_name = "Enemy";
		break;
	}
	case Berserker:
	{
		m_unittype = ut;
		m_owner = ot;
		m_max_hp =10;
		m_hp = 10;
		m_weapon_damage = 8;
		m_armor_defense = 2;
		m_tile_movement = 4;

		m_sprite.setTexture(g_textures.GetTexture("Berserker_enemy"));
		m_owner_name = "Enemy";
		break;
	}
	default:
		break;
	}

	this->setPosition({ static_cast<float>(pos.y * 64 + 32), static_cast<float>(pos.x * 64 + 32) });
	m_sprite.setOrigin(sf::Vector2f(32.f, 32.f));

	g_unit_manager.m_enemy_units.push_back(this);

	g_unit_manager.m_all_units.push_back(this);
}

void EnemyAI::CalculateChoices()
{
	m_choices.clear();

	if (this->m_moved ||this->m_attacked)
		return;

	const static sf::Vector2i directions[4] = { {0,1},{0,-1},{1,0},{-1,0} };
	
	auto nodes = g_Dijkstra.FindAllPaths(this->m_current_tile->m_idx, this->m_tile_movement,Player);

	//Now we check all tiles that can be attacked
	for (auto it : nodes)
	{
		const sf::Vector2i current_cell = it.second->m_cell;

		//Calculate the tile score by using influence maps
		float tile_score = g_influence_map.m_tile_bonus_IM[current_cell.x][current_cell.y] * (m_use_global_parameters ? g_AI.global_tile_IM_weight : m_tile_IM_weight) +
			g_influence_map.m_ally_IM[current_cell.x][current_cell.y] * (m_use_global_parameters ? g_AI.global_ally_IM_weight : m_ally_IM_weight) -
			g_influence_map.m_enemy_IM[current_cell.x][current_cell.y] * (m_use_global_parameters ? g_AI.global_enemy_IM_weight : m_enemy_IM_weight);

		//Check nearest enemy of this tile
		float min_distance = 100.f;
		Unit * nearest_enemy;
		for (auto ir : g_unit_manager.m_player_units)
		{
			float distance = static_cast<float>(std::abs(ir->m_current_tile->m_idx.x - current_cell.x) + std::abs(ir->m_current_tile->m_idx.y - current_cell.y));

			if (distance < min_distance)
			{
				min_distance = distance;
				nearest_enemy = ir;
			}
		}
		//The near the tile to the enemy, the better
		tile_score += (25.f / min_distance) * (m_use_global_parameters ? g_AI.global_nearest_weight : m_nearest_weight);

		m_choices.push_back(Choice{ this, current_cell, tile_score });


		//Check for enemies that can be attacked from this tile
		for (int i = 0; i < 4; i++)
		{
			sf::Vector2i attacking_tile = current_cell + directions[i];

			//If it's an invalid move skip
			if (attacking_tile.x < 0 || attacking_tile.y < 0 ||
				attacking_tile.x >= g_current_grid.height || attacking_tile.y >= g_current_grid.width)
			{
				continue;
			}

			Tile & a_tile = g_current_grid.m_grid[attacking_tile.x][attacking_tile.y];

			//if there is no enemy skip
			if (a_tile.m_occupant == nullptr || a_tile.m_occupant->m_owner == Enemy)
			{
				continue;
			}

			Unit * enemy = a_tile.m_occupant;
			float enemy_score = (m_use_global_parameters ? g_AI.global_fight_weight : m_fight_weight);
			float possible_damage = std::max((static_cast<float>(this->m_weapon_damage) - static_cast<float>(enemy->m_armor_defense + enemy->m_current_tile->m_bonus)),1.f) *
				(m_use_global_parameters ? g_AI.global_damage_weight : m_damage_weight);


			enemy_score += static_cast<float>(1.f / enemy->m_hp) * (m_use_global_parameters ? g_AI.global_health_weight : m_health_weight) + possible_damage;


			m_choices.push_back(Choice{ this, current_cell, tile_score + enemy_score, attacking_tile, enemy });


		}

		
	}

	std::sort(m_choices.begin(), m_choices.end(), [](const Choice & left, const Choice & right) -> bool
	{
		return left.m_score > right.m_score;
	});

}

void StrategicAI::CalculateChoices()
{
	choices.clear();

	for (int i = 0; i < g_unit_manager.m_enemy_units.size(); i++)
	{
		EnemyAI * enemy = dynamic_cast<EnemyAI*>(g_unit_manager.m_enemy_units[i]);
		enemy->CalculateChoices();
		if(!enemy->m_choices.empty())
			choices.push_back(enemy->m_choices[0]);
	}

	if (choices.empty())
		return;

	std::sort(choices.begin(), choices.end(), [](const Choice & left, const Choice & right)
	{
		return left.m_score > right.m_score;
	});



}

void StrategicAI::CalculateChoicesRandom()
{
	choices.clear();

	for (int i = 0; i < g_unit_manager.m_enemy_units.size(); i++)
	{
		EnemyAI * enemy = dynamic_cast<EnemyAI*>(g_unit_manager.m_enemy_units[i]);
		enemy->CalculateChoices();

		if (enemy->m_choices.empty())
			continue;

		if(enemy->m_choices.size() > 2)
			choices.push_back(enemy->m_choices[rand() % 3]);
		else
			choices.push_back(enemy->m_choices[0]);
	}

	if (choices.empty())
		return;

	std::sort(choices.begin(), choices.end(), [](const Choice & left, const Choice & right)
	{
		return left.m_score > right.m_score;
	});
}

void StrategicAI::ChooseBestScore()
{
	if (choices.empty())
		return;

	Choice best_choice = choices[0];

	best_choice.m_executor->MoveTo(&g_current_grid.m_grid[best_choice.m_move_tile.x][best_choice.m_move_tile.y]);

	if(best_choice.m_attack_tile.x != -1)
		best_choice.m_executor->Attack(best_choice.m_objective);

}

void StrategicAI::ChooseBestScoreRandom()
{
	if (choices.empty())
		return;

	Choice best_choice = choices[rand() % choices.size()];

	best_choice.m_executor->MoveTo(&g_current_grid.m_grid[best_choice.m_move_tile.x][best_choice.m_move_tile.y]);

	if (best_choice.m_attack_tile.x != -1)
		best_choice.m_executor->Attack(best_choice.m_objective);
}

bool StrategicAI::EndTurn()
{
	if (choices.empty())
		return true;
	else
		return false;
}
