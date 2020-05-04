/******************************************************************************/
/*!
\file		unit.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Implementation of Unit, the actor used for the Player (and AI, which
			has its own specialized version). It Moves and Attacks through Tiles.
*/
/******************************************************************************/
#include "global.h"
#include "unit.h"
#include "tile.h"

Unit::Unit(UnitType ut, OwnerType ot, sf::Vector2i pos)
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


		m_sprite.setTexture(g_textures.GetTexture("Saber_player"));
		m_owner_name = "Player";

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


		m_sprite.setTexture(g_textures.GetTexture("Lancer_player"));
		m_owner_name = "Player";

		break;
	}
	case Berserker:
	{
		m_unittype = ut;
		m_owner = ot;
		m_max_hp = 10;
		m_hp = 10;
		m_weapon_damage = 8;
		m_armor_defense = 2;
		m_tile_movement = 4;


		m_sprite.setTexture(g_textures.GetTexture("Berserker_player"));
		m_owner_name = "Player";

		break;
	}
	default:
		break;
	}

	this->setPosition({static_cast<float>(pos.y * 64 + 32), static_cast<float>(pos.x * 64 + 32) });
	m_sprite.setOrigin(sf::Vector2f(32.f, 32.f));


	g_unit_manager.m_player_units.push_back(this);


	g_unit_manager.m_all_units.push_back(this);
	
}

std::string Unit::GetInformation()
{
	std::string info;
	info += "Owner: " + m_owner_name + '\n';
	info += "Health: " + std::to_string(m_hp) + "/" + std::to_string(m_max_hp) + '\n';
	return info;
}

void Unit::MoveTo(Tile* tile)
{
	this->m_current_tile->m_occupant = nullptr;

	this->m_current_tile = nullptr;

	this->setPosition(tile->getPosition());

	g_current_grid.SetUnit(this);

	this->m_moved = true;
}

void Unit::Attack(Unit * blocking_unit)
{
	this->m_moved = true;
	this->m_attacked = true;

	//Damage Formula
	blocking_unit->m_hp -= std::max(this->m_weapon_damage - static_cast<int>(blocking_unit->m_armor_defense + blocking_unit->m_current_tile->m_bonus),1);

	if (blocking_unit->m_hp <= 0)
		blocking_unit->Die();
}

void Unit::Die()
{
	for (int i = 0; i < g_unit_manager.m_all_units.size(); i++)
	{
		if (g_unit_manager.m_all_units[i] == this)
			g_unit_manager.m_all_units.erase(g_unit_manager.m_all_units.begin() + i);
	}

	if (this->m_owner == Player)
	{
		for (int i = 0; i < g_unit_manager.m_player_units.size(); i++)
		{
			if(g_unit_manager.m_player_units[i] == this)
				g_unit_manager.m_player_units.erase(g_unit_manager.m_player_units.begin() + i);
		}
	}
	else
	{
		for (int i = 0; i < g_unit_manager.m_enemy_units.size(); i++)
		{
			if(g_unit_manager.m_enemy_units[i] == this)
				g_unit_manager.m_enemy_units.erase(g_unit_manager.m_enemy_units.begin() + i);
		}
	}

	this->m_current_tile->m_occupant = nullptr;
	
	delete this;
}

void UnitManager::Restart(OwnerType owner)
{
	if (owner == Player)
	{
		for (auto it : m_player_units)
		{
			it->m_moved = false;
			it->m_attacked = false;
		}
	}
	else
    {
		for (auto it : m_enemy_units)
		{
			it->m_moved = false;
			it->m_attacked = false;
		}
	}
}

void UnitManager::Clear()
{
	for (auto it : m_all_units)
	{
		it->m_current_tile->m_occupant = nullptr;
		delete it;
	}

	m_all_units.clear();
	m_player_units.clear();
	m_enemy_units.clear();
}
