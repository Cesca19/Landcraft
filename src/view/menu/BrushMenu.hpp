//
// Created by fran on 15/04/2026.
//

#ifndef LANDCRAFT_BRUSHMENU_HPP
#define LANDCRAFT_BRUSHMENU_HPP

#include "../../ui/UIFactory.hpp"

class BrushMenu {
public:
    BrushMenu(sf::Vector2f brushSizeUIStartPosition, const std::vector<std::string> &brushImagePaths);
    ~BrushMenu();
    void setIncrementBrushSizeButtonCallback(const std::function<void()> &callback) const;
    void setDecrementBrushSizeButtonCallback(const std::function<void()> &callback) const;
    void setBrushSizeValueText(const std::string &value) const;
    void setBrushTypeButtonCallback(int index, const std::function<void()> &callback) const;
private:
    void initBrushTypeWidgets(const std::vector<std::string> &brushImagePaths);
    void initBrushSizeWidgets(sf::Vector2f brushSizeUIStartPosition);
    void initWidgetsList();

    sf::Vector2f m_brushSizeUIStartPosition;
    Box *m_brushSizeBox;
    Text *m_brushSizeText;
    Text *m_brushSizeValueText;
    SpriteButton *m_incrementBrushSize;
    SpriteButton *m_decrementBrushSize;
    Box *m_brushTypeBox;
    Text *m_brushTypeText;
    std::vector<SpriteButton *> m_brushTypesButtons;
    std::vector<IWidget *> m_widgets;
};


#endif //LANDCRAFT_BRUSHMENU_HPP