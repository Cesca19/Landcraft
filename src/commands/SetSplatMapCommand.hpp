//
// Created by fran on 03/06/2026.
//

#ifndef LANDCRAFT_SETSPLATMAPCOMMAND_HPP
#define LANDCRAFT_SETSPLATMAPCOMMAND_HPP

#include "ICommand.hpp"
#include <SFML/Graphics/Image.hpp>

class SetSplatMapCommand : public ICommand {
public:
    SetSplatMapCommand(const sf::Image& oldSplatMapImage, const sf::Image& newSplatMapImage);
    void execute(WorldModel &model, WorldView &view) override;
    void undo(WorldModel &model, WorldView &view) override;
    std::string getName() override;
private:
    sf::Image m_oldSplatMapImage;
    sf::Image m_newSplatMapImage;
};


#endif //LANDCRAFT_SETSPLATMAPCOMMAND_HPP