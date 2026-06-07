//
// Created by fran on 03/06/2026.
//

#include "SetSplatMapCommand.hpp"

SetSplatMapCommand::SetSplatMapCommand(const sf::Image &oldSplatMapImage, const sf::Image &newSplatMapImage)
    : m_oldSplatMapImage(oldSplatMapImage)
    , m_newSplatMapImage(newSplatMapImage)
{
}

void SetSplatMapCommand::execute(WorldModel &model, WorldView &view)
{
    const sf::IntRect area(0, 0, m_newSplatMapImage.getSize().x, m_newSplatMapImage.getSize().y);
    view.restoreSplatmapArea(area, m_newSplatMapImage);
    view.updateSplatmapImage();
}

void SetSplatMapCommand::undo(WorldModel &model, WorldView &view)
{
    sf::IntRect area(0, 0, m_oldSplatMapImage.getSize().x, m_oldSplatMapImage.getSize().y);
    view.restoreSplatmapArea(area, m_oldSplatMapImage);
    view.updateSplatmapImage();
}

std::string SetSplatMapCommand::getName()
{
    return "Set Terrain Splat Map";
}