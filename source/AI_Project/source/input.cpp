/******************************************************************************/
/*!
\file		input.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Simple Input Manager
*/
/******************************************************************************/
#include "global.h"
#include "tile.h"
#include "input.h"

#include <imgui-master/imgui.h>

#include <SFML/Window/Event.hpp>

void Input::GetInput(const sf::RenderWindow & window)
{
	if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())
		return;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// get the local mouse position (relative to a window)
		sf::Vector2i temp = sf::Mouse::getPosition(window); // window is a sf::Window

		//If the mouse is outside the window to nothing
		if (temp.x < -1 || temp.y < -1 ||
			temp.x > 1280 || temp.y > 720)
			return;

		localPosition = temp;

		if (m_current_state == Moving)
		{
			Tile* moving_tile = nullptr;

			g_current_grid.GetTile(g_input.localPosition, &moving_tile);

			
			if (moving_tile == nullptr)
			{
				m_current_state = Selecting;
				g_current_grid.RestartColor();
				return;
			}

			//Player clicked on a movable tile
			if (g_Dijkstra.m_path.find(moving_tile->m_idx.x * g_current_grid.height + moving_tile->m_idx.y) != g_Dijkstra.m_path.end())
			{
				Unit * selected_unit = g_grid.selected_tile->m_occupant;

				selected_unit->MoveTo(moving_tile);

				m_current_state = Selecting;
				g_current_grid.RestartColor();
			}
			//Player clicked in nothing, restart everything
			else
			{
				m_current_state = Selecting;
				g_current_grid.RestartColor();
				return;
			}
			


			
		}
		else if (m_current_state == Selecting)
		{
			g_current_grid.GetTile(g_input.localPosition, &g_grid.selected_tile);
		}

		else if (m_current_state == Attacking)
		{
			Tile* attacking_tile;

			g_current_grid.GetTile(g_input.localPosition, &attacking_tile);

			if (attacking_tile == nullptr)
			{
				m_current_state = Selecting;
				g_current_grid.RestartColor();
				return;
			}

			if (attacking_tile->m_occupant == nullptr)
			{
				m_current_state = Selecting;
				g_current_grid.RestartColor();
				return;
			}

			if (attacking_tile->m_occupant->m_owner == Player)
			{
				m_current_state = Selecting;
				g_current_grid.RestartColor();
				return;
			}

			sf::Vector2i vec = { g_grid.selected_tile->m_idx - attacking_tile->m_idx };

			int distance = std::abs(vec.x) + std::abs(vec.y);

			if (distance == 1)
			{
				Unit * selected_unit = g_grid.selected_tile->m_occupant;
				selected_unit->Attack(attacking_tile->m_occupant);
			}
			

			m_current_state = Selecting;
			g_current_grid.RestartColor();

		}

	}

}
