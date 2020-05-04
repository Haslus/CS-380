/******************************************************************************/
/*!
\file		input.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Simple Input Manager
*/
/******************************************************************************/
#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

enum InputState
{
	Selecting,Moving,Attacking
};

class Input
{

public:
	void GetInput(const sf::RenderWindow &);

	sf::Vector2i localPosition = {-1,-1};

	InputState m_current_state;
};