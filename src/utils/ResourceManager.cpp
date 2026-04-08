//
// Created by fran on 08/04/2026.
//

#include "ResourceManager.hpp"

ResourceManager & ResourceManager::getInstance()
{
    static ResourceManager instance; // Created ONLY on the first call
    return instance;
}

sf::Font & ResourceManager::getFont(const std::string &filepath)
{
    auto it = m_fonts.find(filepath);
    if (m_fonts.find(filepath) != m_fonts.end())
        return *(m_fonts[filepath].get());

    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(filepath))
        throw std::runtime_error("Failed to load: " + filepath);

    m_fonts[filepath] = std::move(font);
    return *(m_fonts[filepath].get());
}

sf::Texture & ResourceManager::getTexture(const std::string &filepath)
{
    auto it = m_textures.find(filepath);
    if (m_textures.find(filepath) != m_textures.end())
        return *(m_textures[filepath].get());

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filepath))
        throw std::runtime_error("Failed to load: " + filepath);

    m_textures[filepath] = std::move(texture);
    return *(m_textures[filepath].get());
}

void ResourceManager::clear()
{
    m_fonts.clear();
    m_textures.clear();
    std::cout << "[Resource Manager] All resources cleared." << std::endl;
}
