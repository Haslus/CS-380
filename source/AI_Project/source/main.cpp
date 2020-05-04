/******************************************************************************/
/*!
\file		main.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Main file, where all the demo logic is executed. ImGui logic and
			Rendering also happens here.
*/
/******************************************************************************/
#include "global.h"
#include "tile.h"
#include "input.h"
#include "pathfinding.h"
#include "game.h"
#include "influencemap.h"
#include "enemyAI.h"

#include <imgui-master/imgui.h>
#include <imgui-sfml-master/imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>


int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "");
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);

	sf::Color bgColor;

	float color[3] = { 0.f, 0.f, 0.f };
	char windowTitle[255] = "Demo";

	window.setTitle(windowTitle);
	window.resetGLStates(); // call it if you only draw ImGui. Otherwise not needed.
	sf::Clock deltaClock;

	
	//Add ALL textures that will be used
	//Default Texture
	g_textures.AddTexture("./data/ERROR.png", "ERROR");
	//Unit Textures
	g_textures.AddTexture("./data/Saber_player.png"		, "Saber_player");
	g_textures.AddTexture("./data/Saber_enemy.png"		, "Saber_enemy");
	g_textures.AddTexture("./data/Lancer_player.png"	, "Lancer_player");
	g_textures.AddTexture("./data/Lancer_enemy.png"		, "Lancer_enemy");
	g_textures.AddTexture("./data/Berserker_player.png"	, "Berserker_player");
	g_textures.AddTexture("./data/Berserker_enemy.png"	, "Berserker_enemy");
	//Tile Textures
	g_textures.AddTexture("./data/Grass.png", "Grass");
	g_textures.AddTexture("./data/Mountain.png", "Mountain");
	g_textures.AddTexture("./data/River.png", "River");
	g_textures.AddTexture("./data/Forest.png", "Forest");
	g_textures.AddTexture("./data/SelectedTile.png", "SelectedTile");

	// create the tilemap from the level definition
	TileMap map("./data/map1.txt");
	g_grid.AddTileMap(map);

	//Create a sprite that represents the current selected tile
	sf::Sprite selection;
	selection.setTexture(g_textures.GetTexture("SelectedTile"));
	selection.setOrigin({32,32});

	g_game.m_gamestate = Start;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		g_input.GetInput(window);
		g_current_grid.RestartColor();

		if (g_grid.selected_tile != nullptr)
		{
			selection.setPosition(g_grid.selected_tile->getPosition());
		}
		else
		{
			selection.setPosition({ -100,100 });
		}
		
		ImGui::Begin("Game Window");

		if (g_game.m_gamestate == Start)
		{
			if (ImGui::CollapsingHeader("Start Game"))
			{
				if (ImGui::Button("Start Demo"))
				{
					g_game.StartGame();
				}

				const char* unit_class[] = { "Saber","Lancer","Berserker" };
				static int item_current_1 = 0;
				ImGui::Combo("Choose Class", &item_current_1, unit_class, IM_ARRAYSIZE(unit_class));

				const char* unit_owner[] = { "Player", "Enemy" };
				static int item_current_2 = 0;
				ImGui::Combo("Choose Owner", &item_current_2, unit_owner, IM_ARRAYSIZE(unit_owner));
				if (g_grid.selected_tile == nullptr)
					ImGui::Text("Please select a Tile");

				else
				{
					ImGui::Text("Current Selected Tile Row:%d Column:%d", g_grid.selected_tile->m_idx.x, g_grid.selected_tile->m_idx.y);

					if (ImGui::Button("Add Unit"))
					{
						OwnerType owner = static_cast<OwnerType>(item_current_2);
						Unit * new_unit;
						if (owner == Player)
							new_unit = new Unit(static_cast<UnitType>(item_current_1), owner, g_grid.selected_tile->m_idx);
						else
							new_unit = new EnemyAI(static_cast<UnitType>(item_current_1), owner, g_grid.selected_tile->m_idx);

						g_current_grid.SetUnit(new_unit);

						g_grid.selected_tile = nullptr;
					}
				}
			}

			
		}

		if (ImGui::CollapsingHeader("Game Information"))
		{
			if (g_game.m_gamestate == PlayerTurn)
				ImGui::Text("Your turn.");

			else if (g_game.m_gamestate == EnemyTurn)
				ImGui::Text("Enemy's turn.");

			if (g_game.m_game_started)
			{
				if (ImGui::Button("End Turn"))
				{
					g_game.m_gamestate = EnemyTurn;
					g_unit_manager.Restart(Player);
					g_influence_map.UpdateAllyIM();
					g_influence_map.UpdateEnemyIM();
				}
			}


			if (g_grid.selected_tile != nullptr)
			{
				ImGui::Text("Tile Information");
				ImGui::Text("Current Tile %d %d", g_grid.selected_tile->m_idx.x, g_grid.selected_tile->m_idx.y);
				sf::Vector2i pos = g_grid.selected_tile->m_idx;
				ImGui::Text(std::string("Tile Type: " + g_grid.selected_tile->m_tiletype_name).c_str());

				if (g_grid.selected_tile->m_occupant != nullptr)
				{
					ImGui::Text("Unit Information");
					ImGui::Text(g_grid.selected_tile->m_occupant->GetInformation().c_str());

					if (g_game.m_gamestate == PlayerTurn)
					{
						if (g_grid.selected_tile->m_occupant->m_owner == Player)
						{
							if (!g_grid.selected_tile->m_occupant->m_attacked && !g_grid.selected_tile->m_occupant->m_moved)
							{
								if (ImGui::Button("Move"))
								{
									if (g_input.m_current_state == Moving)
									{
										g_input.m_current_state = Selecting;
										g_current_grid.RestartColor();
									}

									else
									{
										g_input.m_current_state = Moving;


									}
								}
							}

							if (!g_grid.selected_tile->m_occupant->m_attacked)
							{
								if (ImGui::Button("Attack"))
								{
									g_input.m_current_state = Attacking;
								}
							}


						}
					}


				}

			}
		}

		if (g_game.m_game_started && g_game.m_gamestate == PlayerTurn)
		{
			if (ImGui::CollapsingHeader("Influence Map"))
			{
				static bool show_number;
				static bool show_tile;
				static bool show_ally;
				static bool show_enemy;
				ImGui::Checkbox("Show Data", &show_number);
				ImGui::Checkbox("Visualize Tile Influence Map", &show_tile);
				ImGui::Checkbox("Visualize Ally (Enemy) Influence Map", &show_ally);
				ImGui::Checkbox("Visualize Enemy (Player) Influence Map", &show_enemy);

				if (show_number)
				{
					std::string tiles1;
					for (int i = 0; i < g_current_grid.height; i++)
					{
						for (int j = 0; j < g_current_grid.width; j++)
						{
							tiles1 += std::to_string(g_influence_map.m_tile_bonus_IM[i][j]) + ' ';
						}
						tiles1 += '\n';
					}

					ImGui::Text(tiles1.c_str());

					std::string tiles2;
					for (int i = 0; i < g_current_grid.height; i++)
					{
						for (int j = 0; j < g_current_grid.width; j++)
						{
							tiles2 += std::to_string(g_influence_map.m_ally_IM[i][j]) + ' ';
						}
						tiles2 += '\n';
					}

					ImGui::Text(tiles2.c_str());

					std::string tiles3;
					for (int i = 0; i < g_current_grid.height; i++)
					{
						for (int j = 0; j < g_current_grid.width; j++)
						{
							tiles3 += std::to_string(g_influence_map.m_enemy_IM[i][j]) + ' ';
						}
						tiles3 += '\n';
					}

					ImGui::Text(tiles3.c_str());
				}

				if (show_tile)
				{
					for (int i = 0; i < g_current_grid.height; i++)
					{
						for (int j = 0; j < g_current_grid.width; j++)
						{
							sf::Color col{ static_cast<sf::Uint8>((g_influence_map.m_tile_bonus_IM[i][j] + 1) * 40),0 ,0 };
							g_current_grid.m_grid[i][j].m_sprite.setColor(col);
						}
					}
				}

				if (show_ally)
				{
					for (int i = 0; i < g_current_grid.height; i++)
					{
						for (int j = 0; j < g_current_grid.width; j++)
						{
							if (g_influence_map.m_ally_IM[i][j] == 0.f)
								continue;
							sf::Color col{ 0,static_cast<sf::Uint8>((g_influence_map.m_ally_IM[i][j]) * 200) ,0 };
							g_current_grid.m_grid[i][j].m_sprite.setColor(col);
						}
					}
				}

				if (show_enemy)
				{
					for (int i = 0; i < g_current_grid.height; i++)
					{
						for (int j = 0; j < g_current_grid.width; j++)
						{
							if (g_influence_map.m_enemy_IM[i][j] == 0.f)
								continue;
							sf::Color col{ 0,0 ,static_cast<sf::Uint8>((g_influence_map.m_enemy_IM[i][j]) * 200) };
							g_current_grid.m_grid[i][j].m_sprite.setColor(col);
						}
					}
				}
			}
			
			if (ImGui::CollapsingHeader("AI Parameters"))
			{

				ImGui::Text("Global AI");
				ImGui::Checkbox("Choose Between Top 3 Choice (Instead of Best Choice)", &g_AI.random_choice);
				ImGui::Checkbox("Random Choice Between All Choices (Instead of Best Choice Between All)", &g_AI.random_score);
				ImGui::PushItemWidth(100);
				ImGui::DragFloat("Global Weight for Tile Influence Map", &g_AI.global_tile_IM_weight, 0.1f, -100.f, 100.f);
				ImGui::DragFloat("Global Weight for Ally Influence Map", &g_AI.global_ally_IM_weight, 0.1f, -100.f, 100.f);
				ImGui::DragFloat("Global Weight for Enemy Influence Map", &g_AI.global_enemy_IM_weight, 0.1f, -100.f, 100.f);
				ImGui::DragFloat("Global Weight for Nearest Enemy", &g_AI.global_nearest_weight, 0.1f, -100.f, 100.f);
				ImGui::DragFloat("Global Weight for Entering Fight", &g_AI.global_fight_weight, 0.1f, -100.f, 100.f);
				ImGui::DragFloat("Global Weight for Enemy Health", &g_AI.global_health_weight, 0.1f, -100.f, 100.f);
				ImGui::DragFloat("Global Weight for Combat Damage", &g_AI.global_damage_weight, 0.1f, -100.f, 100.f);
				ImGui::PopItemWidth();
				ImGui::Text("Local AI");

				if (g_grid.selected_tile != nullptr)
					if (g_grid.selected_tile->m_occupant != nullptr && g_grid.selected_tile->m_occupant->m_owner == Enemy)
					{
						ImGui::Checkbox("Use Global", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_use_global_parameters);

						if (dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_use_global_parameters == false)
						{
							ImGui::PushItemWidth(100);
							ImGui::DragFloat("Weight for Tile Influence Map", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_tile_IM_weight, 0.1f, -100.f, 100.f);
							ImGui::DragFloat("Weight for Ally Influence Map", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_ally_IM_weight, 0.1f, -100.f, 100.f);
							ImGui::DragFloat("Weight for Enemy Influence Map", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_enemy_IM_weight, 0.1f, -100.f, 100.f);
							ImGui::DragFloat("Weight for Nearest Enemy", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_nearest_weight, 0.1f, -100.f, 100.f);
							ImGui::DragFloat("Weight for Entering Fight", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_fight_weight, 0.1f, -100.f, 100.f);
							ImGui::DragFloat("Weight for Enemy Health", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_health_weight, 0.1f, -100.f, 100.f);
							ImGui::DragFloat("Weight for Combat Damage", &dynamic_cast<EnemyAI*>(g_grid.selected_tile->m_occupant)->m_damage_weight, 0.1f, -100.f, 100.f);
							ImGui::PushItemWidth(100);
						}

					}
			}

		}

		
		
		ImGui::End();
		

		if(g_input.m_current_state == Moving)
			g_current_grid.ShowAvailableMovement(g_Dijkstra.FindAllPaths(g_grid.selected_tile->m_idx, g_grid.selected_tile->m_occupant->m_tile_movement, Enemy));

		if(g_input.m_current_state == Attacking)
			g_current_grid.ShowAvailableAttacks(g_grid.selected_tile->m_occupant);

		if (g_game.m_gamestate == EnemyTurn)
			g_game.UpdateGame(window);
		else if (g_game.m_gamestate == End)
			g_game.EndGame();


		window.clear(bgColor); // fill background with color

		//Draw the current map
		g_grid.DrawCurrentTileMap(window);
		//Draw all units
		for (auto it : g_unit_manager.m_all_units)
			window.draw(*it);

		window.draw(selection);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
	g_influence_map.Clear();
	g_unit_manager.Clear();
	g_Dijkstra.Clear();
	g_current_grid.Clear();
	return 0;
}