/******************************************************************************/
/*!
\file		game.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Game is defined here, which defines the start, development and end of
			a game, which is considered a skirmish between Player and AI. It ends
			when one of them is out of units.
*/
/******************************************************************************/
#include "global.h"
#include "game.h"
#include "tile.h"
#include "influencemap.h"
#include "enemyAI.h"


const float dt = 0.016f;
void Game::StartGame()
{
	m_gamestate = PlayerTurn;
	m_game_started = true;
	g_influence_map.CreateInitialIM();
	m_timer = 0;
}

void Game::UpdateGame(sf::RenderWindow & window)
{
	m_timer += dt;

	if (m_timer > 1.f && m_gamestate == EnemyTurn)
	{
		m_timer = 0;
		if (g_AI.random_choice)
			g_AI.CalculateChoicesRandom();
		else
			g_AI.CalculateChoices();

		if (g_AI.random_score)
			g_AI.ChooseBestScoreRandom();
		else
			g_AI.ChooseBestScore();

		g_influence_map.UpdateAllyIM();
		g_influence_map.UpdateEnemyIM();

		if (g_AI.EndTurn())
		{
			m_gamestate = PlayerTurn;
			g_unit_manager.Restart(Enemy);
		}


	}

	if (g_unit_manager.m_player_units.empty() || g_unit_manager.m_enemy_units.empty())
	{
		m_gamestate = End;
	}
}

void Game::EndGame()
{
	m_gamestate = Start;
	m_game_started = false;
	g_unit_manager.Clear();
}
