/******************************************************************************/
/*!
\file		resources.cpp
\project	CS380 AI Project
\author		Asier Bilbao
\summary	Simple Texture Manager.
*/
/******************************************************************************/
#include "resources.h"

sf::Texture& Textures::GetTexture(const std::string & name)
{
	auto it = m_textures.find(name);

	if (it != m_textures.end())
		return it->second;
	else
		return m_textures["ERROR"];

}

void Textures::AddTexture(const std::string & path, const std::string & name)
{
	sf::Texture texture;
	texture.loadFromFile(path);
	m_textures[name] = texture;

}
