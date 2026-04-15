//
// Created by fran on 15/04/2026.
//

#ifndef LANDCRAFT_BRUSHMENU_HPP
#define LANDCRAFT_BRUSHMENU_HPP

#include "../../ui/UIFactory.hpp"

class BrushMenu {
public:
    BrushMenu(sf::Vector2f brushSizeUIStartPosition);
    ~BrushMenu();
    void setIncrementBrushSizeButtonCallback(std::function<void()> callback);
    void setDecrementBrushSizeButtonCallback(std::function<void()> callback);
    void setBrushSizeValueText(const std::string &value);
private:
    void initBrushSizeWidgets(sf::Vector2f brushSizeUIStartPosition);
    void initButtonStyle(SpriteButton *button, HighlightTextAlign align = HighlightTextAlign::Top);
    void initWidgetsList();

    sf::Vector2f m_brushSizeUIStartPosition;
    Box *m_brushSizeBox;
    Text *m_brushSizeText;
    Text *m_brushSizeValueText;
    SpriteButton *m_incrementBrushSize;
    SpriteButton *m_decrementBrushSize;
    std::vector<IWidget *> m_widgets;
};


#endif //LANDCRAFT_BRUSHMENU_HPP