/******************************************************************************/
/*!
\file		resources.h
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Simple Texture Manager.
*/
/******************************************************************************/
#pragma once
#include <map>
#include <SFML/Graphics.hpp>

class Textures
{
public:
	sf::Texture& GetTexture(const std::string & path);
	void AddTexture(const std::string & path, const std::string & name);

private:
	std::map<std::string, sf::Texture> m_textures;
};