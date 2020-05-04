/******************************************************************************/
/*!
\file		global.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Global Singletons, used throughout the program
*/
/******************************************************************************/
#pragma once
#include "singleton.h"
#include "resources.h"

#define g_textures Singleton<Textures>::GetInstance()
#define g_input Singleton<Input>::GetInstance()
#define g_grid Singleton<MasterTileMap>::GetInstance()
#define g_Dijkstra Singleton<Dijkstra>::GetInstance()
#define g_current_grid Singleton<MasterTileMap>::GetInstance().m_tilemaps[Singleton<MasterTileMap>::GetInstance().m_current_tilemap]
#define g_game Singleton<Game>::GetInstance()
#define g_unit_manager Singleton<UnitManager>::GetInstance()
#define g_influence_map Singleton<InfluenceMap>::GetInstance()
#define g_AI Singleton<StrategicAI>::GetInstance()





