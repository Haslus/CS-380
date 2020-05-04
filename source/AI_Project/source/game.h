/******************************************************************************/
/*!
\file		game.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Game is defined here, which defines the start, development and end of
			a game, which is considered a skirmish between Player and AI. It ends
			when one of them is out of units.
*/
/******************************************************************************/
#pragma once

enum GameState
{
	Start,PlayerTurn,EnemyTurn,End
};

class Game
{
public:
	void StartGame();

	void UpdateGame(sf::RenderWindow & window);

	void EndGame();

	GameState m_gamestate = Start;

	bool m_game_started = false;

	float m_timer;
};
