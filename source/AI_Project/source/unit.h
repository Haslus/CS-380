/******************************************************************************/
/*!
\file		unit.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Implementation of Unit, the actor used for the Player (and AI, which
			has its own specialized version). It Moves and Attacks through Tiles.
*/
/******************************************************************************/
#pragma once
#include <SFML/Graphics.hpp>

class Tile;

enum UnitType
{
	Saber = 0, Lancer = 1, Berserker = 2
};

enum OwnerType
{
	Player = 0, Enemy = 1,
};

class Unit : public sf::Drawable, public sf::Transformable
{
public:
	Unit() {};
	Unit(UnitType, OwnerType, sf::Vector2i pos);
	virtual ~Unit() {};

	std::string GetInformation();
	void MoveTo(Tile*);
	void Attack(Unit *);
	void Die();

	OwnerType m_owner;
	Tile * m_current_tile;

	bool m_moved = false;
	bool m_attacked = false;

	sf::Sprite m_sprite;

	UnitType m_unittype;
	std::string m_owner_name;
	int m_max_hp;
	int m_hp;
	int m_weapon_damage;
	int m_armor_defense;
	int m_tile_movement;

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the entity's transform -- combine it with the one that was passed by the caller
		states.transform *= getTransform(); // getTransform() is defined by sf::Transformable
		// You can draw other high-level objects
		target.draw(m_sprite, states);
	}

	
	

	
};


class UnitManager
{
public:
	void Restart(OwnerType owner);
	void Clear();

	std::vector<Unit*> m_all_units;
	std::vector<Unit*> m_player_units;
	std::vector<Unit*> m_enemy_units;

};