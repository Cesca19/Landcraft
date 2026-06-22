//
// Created by fran on 22/06/2026.
//

#include "SetHeightTool.hpp"
#include <iostream>

SetHeightTool::SetHeightTool(sf::Vector2f startMenuPosition)
    : m_isEditing(false)
    , m_isSelectionLocked(false)
    , m_currentSelectionMode(-1)
    , m_heightValue(5)
{
    m_selectionModes.push_back(SelectionMode::TILE_CORNER);
    m_selectionModes.push_back(SelectionMode::TILE);
    initMenuUI(startMenuPosition);
    initOnSetHeightValidateCallback([this] (const std::string& newValue) {
        this->updateHeightValue(newValue);
    });
    setUIVisibility(false);
    setSelectionMode(0);
}

SetHeightTool::~SetHeightTool()
{
}

bool SetHeightTool::isEditing() const
{
    return m_isEditing;
}

bool SetHeightTool::isSelectionLocked() const
{
    return m_isSelectionLocked;
}

SelectionMode SetHeightTool::getRequiredSelectionMode() const 
{
    return m_selectionModes[m_currentSelectionMode];
}

void SetHeightTool::onToolSelected() const
{
    setUIVisibility(true);
}

void SetHeightTool::onToolUnSelected() const
{
    setUIVisibility(false);
}

bool SetHeightTool::areEditableTilesVisible() const
{
    return true;
}

void SetHeightTool::handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view,
                                 BrushController &brushController, CommandHistory &history)
{
}

void SetHeightTool::handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController,
                                           CommandHistory &history)
{
}

void SetHeightTool::setVisibility(bool isVisible) const
{
    setUIVisibility(isVisible);
}

void SetHeightTool::initOnSetHeightValidateCallback(std::function<void(const std::string&)> callback)
{
    m_setHeightInput->initOnValidateCallback(callback);
}

std::string SetHeightTool::getSetHeightInputValue() const
{
    return m_setHeightInput->getText();
}

void SetHeightTool::updateHeightValue(std::string newValue)
{
    m_heightValue = std::stof(newValue);
    std::cout << "Height value updated to: " << m_heightValue << std::endl;
}

void SetHeightTool::setSelectionMode(int index)
{
    if (index < 0 || index >= static_cast<int>(m_selectionModes.size()) || index == m_currentSelectionMode)
        return;
    if (m_currentSelectionMode != -1)
         m_selectionModesButtons[m_currentSelectionMode]->setSelected(false);
    m_currentSelectionMode = index;
    m_selectionModesButtons[m_currentSelectionMode]->setSelected(true);
}

void SetHeightTool::setUIVisibility(bool isVisible) const
{
    for (const auto widget : m_widgets)
        widget->setVisibility(isVisible);
}

void SetHeightTool::initMenuUI(sf::Vector2f startMenuPosition)
{
    m_setHeightToolBox = UIFactory::createBox(startMenuPosition, {190, 310});
    UIFactory::applyDefaultBoxStyle(m_setHeightToolBox);

    m_setHeightToolText = UIFactory::createText(startMenuPosition + sf::Vector2f(35, 10),"Set Height", 20);
    UIFactory::applyDefaultTextStyle(m_setHeightToolText, UIFactory::TextVariant::Title);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(35, 100);
    initSelectionModeUI(startMenuPosition + sf::Vector2f(0, 15));
    initSetHeightUI(startMenuPosition + sf::Vector2f(0, 35));
    initWidgetsList();
}

void SetHeightTool::initSelectionModeUI(sf::Vector2f startMenuPosition)
{
    m_selectionModeBox = UIFactory::createBox(startMenuPosition + sf::Vector2f(20, 45), {150, 130});
    UIFactory::applyDefaultBoxStyle(m_selectionModeBox);

    m_selectionModeText =  UIFactory::createText(startMenuPosition + sf::Vector2f(35, 55),"Selection Mode", 15);
    UIFactory::applyDefaultTextStyle(m_selectionModeText, UIFactory::TextVariant::Label);

    const sf::Vector2f startBtnPosition = startMenuPosition + sf::Vector2f(35, 100);
    SpriteButton *tileCornerMode = UIFactory::createSpriteButton("assets/textures/ui/corner_512.png", startBtnPosition + sf::Vector2f(0, 0),
        sf::Vector2f(32, 32), "Corner", 15);
    SpriteButton *tileMode = UIFactory::createSpriteButton("assets/textures/ui/tiles_512.png", startBtnPosition + sf::Vector2f(65, 0),
        sf::Vector2f(32, 32), "Tile", 15);

    m_selectionModesButtons.push_back(tileCornerMode);
    m_selectionModesButtons.push_back(tileMode);

    for (int i = 0; i < m_selectionModesButtons.size(); i++) {
        UIFactory::applyDefaultSpriteButtonStyle(m_selectionModesButtons[i]);
        m_selectionModesButtons[i]->initOnClickCallback([this, i] () {
            this->setSelectionMode(i);
        });
    }
}

void SetHeightTool::initSetHeightUI(sf::Vector2f startMenuPosition)
{
    m_setHeightText = UIFactory::createText(startMenuPosition + sf::Vector2f(20, 180),"Enter height value", 15);
    UIFactory::applyDefaultTextStyle(m_setHeightText, UIFactory::TextVariant::Label);

    m_setHeightInput = UIFactory::createTextInput(startMenuPosition + sf::Vector2f(20, 210), {150, 40}, MathUtils::toString(m_heightValue), 18, true);
    m_setHeightInput->initCharacterLimit(7);
    m_setHeightInput->setAllowFloatInput(true);
    m_setHeightInput->setAllowNegativeInput(true);
    UIFactory::applyDefaultTextInputStyle(m_setHeightInput);
}

void SetHeightTool::initWidgetsList()
{
    m_widgets.push_back(m_setHeightToolBox);
    m_widgets.push_back(m_setHeightToolText);
    m_widgets.push_back(m_selectionModeBox);
    m_widgets.push_back(m_selectionModeText);
    for (SpriteButton *button : m_selectionModesButtons)
        m_widgets.push_back(button);
    m_widgets.push_back(m_setHeightInput);
    m_widgets.push_back(m_setHeightText);
}