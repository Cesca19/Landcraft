//
// Created by fran on 15/05/2026.
//

#ifndef LANDCRAFT_PAINTSPLATMAPCOMMAND_HPP
#define LANDCRAFT_PAINTSPLATMAPCOMMAND_HPP

#include <vector>
#include <limits>
#include <algorithm>
#include "ICommand.hpp"
#include "../model/WorldModel.hpp"
#include "../model/PaintStroke.hpp"
#include "../view/world/WorldView.hpp"
#include "../utils/MathUtils.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>

class PaintSplatMapCommand : public ICommand {
public:
    PaintSplatMapCommand();
    void addStroke(const PaintStroke& stroke);
    bool isEmpty() const;
    void drawRealTime(WorldModel& model, WorldView& view);

    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;

    std::string getName() override;
private:
    std::vector<PaintStroke> m_strokes;
    sf::Image m_oldPixels;
    sf::IntRect m_modifiedArea;
    bool m_isFirstExecution;
};

#endif //LANDCRAFT_PAINTSPLATMAPCOMMAND_HPP