//
// Created by fran on 14/04/2026.
//

#include "EditionMenu.hpp"

EditionMenu::EditionMenu(sf::Vector2f globalUIPosition)
    : m_editionToolsBoxPosition(0, 250)
    , m_globalUIStartPosition(globalUIPosition)
    , m_toolsMenuStartPosition(m_editionToolsBoxPosition + sf::Vector2f(90, 0))
{
    m_undoButton = UIFactory::createSpriteButton("assets/textures/ui/undo_512.png",
        globalUIPosition + sf::Vector2f(15, 16), {28, 28}, "Undo", 12);
    m_redoButton = UIFactory::createSpriteButton("assets/textures/ui/redo_512.png",
        globalUIPosition + sf::Vector2f(85, 16), {28, 28}, "Redo", 12);
    m_editionToolsBox = UIFactory::createBox(m_editionToolsBoxPosition, {85, 220});
    UIFactory::applyDefaultBoxStyle(m_editionToolsBox);
    m_toolsText = UIFactory::createText(m_editionToolsBoxPosition + sf::Vector2f(15, 10), "Tools:", 20);
    UIFactory::applyDefaultTextStyle(m_toolsText, UIFactory::TextVariant::Title);
    const sf::Vector2f toolStartPos = m_editionToolsBoxPosition + sf::Vector2f(15, 60);
    SpriteButton *elevationButton = UIFactory::createSpriteButton("assets/textures/ui/elevation_tool_512.png", toolStartPos, {32, 32}, "Elevation", 15);
    SpriteButton *paintButton = UIFactory::createSpriteButton("assets/textures/ui/paint_palette_64.png", toolStartPos + sf::Vector2f(0, 80), {32, 32}, "Paint", 15);
    m_editionToolsButtons.emplace_back(elevationButton);
    m_editionToolsButtons.emplace_back(paintButton);
    applyUIStyle();
    initWidgetsList();
}

EditionMenu::~EditionMenu()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void EditionMenu::selectEditionTool(const int toolId) const
{
    m_editionToolsButtons[toolId]->setSelected(true);
}

void EditionMenu::unselectEditionTool(const int toolId) const
{
    m_editionToolsButtons[toolId]->setSelected(false);
}

void EditionMenu::setEditionToolButtonOnCLickCallback(const int toolId, const std::function<void()> &callback) const
{
    m_editionToolsButtons[toolId]->initOnClickCallback(callback);
}

void EditionMenu::setUndoButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_undoButton->initOnClickCallback(callback);
}

void EditionMenu::setRedoButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_redoButton->initOnClickCallback(callback);
}

void EditionMenu::applyUIStyle()
{
    std::vector<SpriteButton*> buttons = { m_undoButton, m_redoButton };
    buttons.insert(buttons.end(), m_editionToolsButtons.begin(), m_editionToolsButtons.end());
    for (SpriteButton* button : buttons) {
        UIFactory::applyDefaultSpriteButtonStyle(button, HighlightTextAlign::Down);
    }
    buttons.clear();
}

void EditionMenu::initWidgetsList()
{
    for (const auto& button : m_editionToolsButtons)
        m_widgets.push_back(button);
    m_widgets.push_back(m_undoButton);
    m_widgets.push_back(m_redoButton);
    m_widgets.push_back(m_toolsText);
    m_widgets.push_back(m_editionToolsBox);
}
