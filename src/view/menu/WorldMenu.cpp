//
// Created by fran on 07/05/2026.
//

#include "WorldMenu.hpp"

WorldMenu::WorldMenu(const sf::Vector2f globalUIPosition, const sf::Vector2f &quitMenuPosition)
    : m_isQuitMenuVisible(true)
{
    m_globalMenuBox = UIFactory::createBox(globalUIPosition, {225, 90});
    UIFactory::applyDefaultBoxStyle(m_globalMenuBox);

    m_quitMenuBox = UIFactory::createBox(quitMenuPosition, {400, 200});
    UIFactory::applyDefaultBoxStyle(m_quitMenuBox);
    m_quitMenuTitle = UIFactory::createText(quitMenuPosition + sf::Vector2f(120, 20), "Quit Landcraft ?", 20);
    UIFactory::applyDefaultTextStyle(m_quitMenuTitle, UIFactory::TextVariant::Title);
    m_quitMenuDescription = UIFactory::createText(quitMenuPosition + sf::Vector2f(20, 60), "Do you want to save your progress before quitting ?", 15);
    UIFactory::applyDefaultTextStyle(m_quitMenuDescription, UIFactory::TextVariant::Default);

    m_saveMapButton = UIFactory::createTextButton(quitMenuPosition + sf::Vector2f(230, 100), "Save and Quit", 17);
    UIFactory::applyDefaultTextButtonStyle(m_saveMapButton, UIFactory::TextVariant::Default);
    m_dontSaveButton = UIFactory::createTextButton(quitMenuPosition + sf::Vector2f(40, 100), "Don't Save and Quit", 16);
    UIFactory::applyDefaultTextButtonStyle(m_dontSaveButton, UIFactory::TextVariant::Default);
    m_cancelButton = UIFactory::createTextButton(quitMenuPosition + sf::Vector2f(180, 150), "Cancel", 17);
    UIFactory::applyDefaultTextButtonStyle(m_cancelButton, UIFactory::TextVariant::Default);

    const sf::Vector2f wireframeShadedModeBoxPosition = globalUIPosition + sf::Vector2f(770, 0);
    m_wireframeShadedModeBox = UIFactory::createBox(wireframeShadedModeBoxPosition, {300, 90});
    UIFactory::applyDefaultBoxStyle(m_wireframeShadedModeBox);
    m_drawModeTitle = UIFactory::createText(wireframeShadedModeBoxPosition + sf::Vector2f(10, 25), " Draw   \nModes ", 15);
    UIFactory::applyDefaultTextStyle(m_drawModeTitle, UIFactory::TextVariant::Label);
    m_wireframeModeButton = UIFactory::createSpriteButton("assets/textures/ui/wireframe_512.png", 
        wireframeShadedModeBoxPosition + sf::Vector2f(230, 20), {28, 28}, "Wireframe", 12);
    m_shadedModeButton = UIFactory::createSpriteButton("assets/textures/ui/shaded_512.png", 
        wireframeShadedModeBoxPosition + sf::Vector2f(155, 20), {28, 28}, "Shaded", 12);
    m_wireframeShadedModeButton = UIFactory::createSpriteButton("assets/textures/ui/wireframe-shaded_512.png", 
        wireframeShadedModeBoxPosition + sf::Vector2f(80, 20), {28, 28}, "Wireframe/Shaded", 12);

    UIFactory::applyDefaultSpriteButtonStyle(m_wireframeModeButton, HighlightTextAlign::Down, false);
    UIFactory::applyDefaultSpriteButtonStyle(m_shadedModeButton, HighlightTextAlign::Down, false);
    UIFactory::applyDefaultSpriteButtonStyle(m_wireframeShadedModeButton, HighlightTextAlign::Down, false);

    m_drawModeButtons.emplace(DrawMode::SHADED, m_shadedModeButton);
    m_drawModeButtons.emplace(DrawMode::WIREFRAME, m_wireframeModeButton);
    m_drawModeButtons.emplace(DrawMode::WIREFRAME_SHADED, m_wireframeShadedModeButton);
}

WorldMenu::~WorldMenu()
{
    for (auto *widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void WorldMenu::setSaveMapButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_saveMapButton->initOnClickCallback(callback);
}

void WorldMenu::setDontSaveButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_dontSaveButton->initOnClickCallback(callback);
}

void WorldMenu::setCancelButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_cancelButton->initOnClickCallback(callback);
}

void WorldMenu::setQuitMenuVisibility(const bool isVisible)
{
    m_quitMenuBox->setVisibility(isVisible);
    m_quitMenuTitle->setVisibility(isVisible);
    m_quitMenuDescription->setVisibility(isVisible);
    m_saveMapButton->setVisibility(isVisible);
    m_dontSaveButton->setVisibility(isVisible);
    m_cancelButton->setVisibility(isVisible);
    m_isQuitMenuVisible = isVisible;
}

bool WorldMenu::isQuitMenuVisible() const
{
    return m_isQuitMenuVisible;
}

void WorldMenu::setQuitMenuPosition(const sf::Vector2f &position) const
{
    m_quitMenuBox->setPosition(position);
    m_quitMenuTitle->setPosition(position + sf::Vector2f(120, 20));
    m_quitMenuDescription->setPosition(position + sf::Vector2f(20, 60));
    m_saveMapButton->setPosition(position + sf::Vector2f(230, 100));
    m_dontSaveButton->setPosition(position + sf::Vector2f(40, 100));
    m_cancelButton->setPosition(position + sf::Vector2f(180, 150));
}

void WorldMenu::setDrawModeButtonOnClickCallback(const DrawMode mode, const std::function<void()> &callback)
{
    m_drawModeButtons[mode]->initOnClickCallback(callback);
}

void WorldMenu::selectDrawModeButton(const DrawMode mode)
{
    m_drawModeButtons[mode]->setSelected(true);
}

void WorldMenu::unselectDrawModeButton(const DrawMode mode)
{
    m_drawModeButtons[mode]->setSelected(false);
}

void WorldMenu::initWidgetsList()
{
    m_widgets.push_back(m_globalMenuBox);
    m_widgets.push_back(m_quitMenuBox);
    m_widgets.push_back(m_quitMenuTitle);
    m_widgets.push_back(m_quitMenuDescription);
    m_widgets.push_back(m_saveMapButton);
    m_widgets.push_back(m_dontSaveButton);
    m_widgets.push_back(m_cancelButton);
    m_widgets.push_back(m_wireframeShadedModeBox);
    m_widgets.push_back(m_wireframeModeButton);
    m_widgets.push_back(m_shadedModeButton);
    m_widgets.push_back(m_wireframeShadedModeButton);
    m_widgets.push_back(m_drawModeTitle);
}
