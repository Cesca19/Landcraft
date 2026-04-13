//
// Created by fran on 08/04/2026.
//

#ifndef LANDCRAFT_RESOURCEMANAGER_HPP
#define LANDCRAFT_RESOURCEMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <stdexcept>

class ResourceManager {
public:
    static ResourceManager& getInstance();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    sf::Font& getFont(const std::string& filepath);
    sf::Texture& getTexture(const std::string& filepath, bool isSmooth = false);
    sf::Image& getImage(const std::string& filepath);
    void clear();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Image>> m_images;
};



#endif //LANDCRAFT_RESOURCEMANAGER_HPP
