/******************************************************************************/
/*!
\file		enemyAI.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Enemy AI is defined here, the Unit used for the AI. Strategic AI is
			also defined here.
*/
/******************************************************************************/
#pragma once
#include "unit.h"


class Choice
{
public:
	Choice(Unit * exec, sf::Vector2i move, float score, sf::Vector2i attack = { -1,-1 }, Unit * obj = nullptr) : m_executor(exec),
		m_move_tile(move), m_attack_tile(attack), m_objective(obj), m_score(score) {};

	Unit * m_executor;
	sf::Vector2i m_move_tile;
	sf::Vector2i m_attack_tile;
	Unit* m_objective;
	float m_score;
};

class EnemyAI : public Unit
{
public:
	EnemyAI(UnitType, OwnerType, sf::Vector2i pos);

	void CalculateChoices();
	std::vector<Choice> m_choices;
	Unit* m_objective;

	float m_tile_IM_weight = 1.f;
	float m_ally_IM_weight = 1.f;
	float m_enemy_IM_weight = 1.f;
	float m_nearest_weight = 1.f;
	float m_fight_weight  = 1.f;
	float m_health_weight = 1.f;
	float m_damage_weight = 1.f;

	bool m_use_global_parameters = false;
	
};

class StrategicAI
{
public:
	void CalculateChoices();
	void CalculateChoicesRandom();
	void ChooseBestScore();
	void ChooseBestScoreRandom();
	bool EndTurn();

	bool random_score;
	bool random_choice;

	std::vector<Choice> choices;

	float global_tile_IM_weight = 1.f;
	float global_ally_IM_weight = 1.f;
	float global_enemy_IM_weight = 1.f;
	float global_nearest_weight = 1.f;
	float global_fight_weight = 1.f;
	float global_health_weight = 1.f;
	float global_damage_weight = 1.f;

};

