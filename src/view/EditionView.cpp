//
// Created by fran on 14/04/2026.
//

#include "EditionView.hpp"

EditionView::EditionView(sf::Vector2f globalUIPosition)
    : m_editionToolsBoxPosition(0, 250)
    , m_globalUIStartPosition(globalUIPosition)
    , m_toolsMenuStartPosition(m_editionToolsBoxPosition + sf::Vector2f(90, 0))
{
    m_undoButton = UIFactory::createSpriteButton("assets/textures/ui/undo_512.png",
        globalUIPosition + sf::Vector2f(15, 16), {28, 28}, "Undo", 12);
    m_redoButton = UIFactory::createSpriteButton("assets/textures/ui/redo_512.png",
        globalUIPosition + sf::Vector2f(85, 16), {28, 28}, "Redo", 12);
    m_editionToolsBox = UIFactory::createBox(m_editionToolsBoxPosition, {85, 210});
    m_editionToolsBox->initColors(sf::Color(205, 185, 220), sf::Color(255, 255, 255));
    m_toolsText = UIFactory::createText(m_editionToolsBoxPosition + sf::Vector2f(15, 10), "Tools:", 20);
    m_toolsText->init(sf::Color(123, 101, 81), sf::Text::Bold | sf::Text::Underlined);
    const sf::Vector2f toolStartPos = m_editionToolsBoxPosition + sf::Vector2f(15, 60);
    SpriteButton *elevationButton = UIFactory::createSpriteButton("assets/textures/ui/elevation_tool_512.png", toolStartPos, {32, 32}, "Elevation", 15);
    SpriteButton *paintButton = UIFactory::createSpriteButton("assets/textures/ui/paint_palette_64.png", toolStartPos + sf::Vector2f(0, 80), {32, 32}, "Paint", 15);
    m_editionToolsButtons.emplace_back(elevationButton);
    m_editionToolsButtons.emplace_back(paintButton);
    applyUIStyle();
    initWidgetsList();
}

EditionView::~EditionView()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void EditionView::selectEditionTool(const int toolId) const
{
    m_editionToolsButtons[toolId]->setSelected(true);
}

void EditionView::unselectEditionTool(const int toolId) const
{
    m_editionToolsButtons[toolId]->setSelected(false);
}

void EditionView::setEditionToolButtonOnCLickCallback(const int toolId, const std::function<void()> &callback) const
{
    m_editionToolsButtons[toolId]->initOnClickCallback(callback);
}

void EditionView::setUndoButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_undoButton->initOnClickCallback(callback);
}

void EditionView::setRedoButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_redoButton->initOnClickCallback(callback);
}

void EditionView::applyUIStyle()
{
    std::vector<SpriteButton*> buttons = { m_undoButton, m_redoButton };
    buttons.insert(buttons.end(), m_editionToolsButtons.begin(), m_editionToolsButtons.end());
    for (SpriteButton* button : buttons) {
        button->initOutlineStatesColors(sf::Color(255, 255, 255, 175), sf::Color(178, 247, 239),
            sf::Color(115, 80, 135), sf::Color(255, 255, 255, 225), sf::Color(123, 101, 81));
        button->initBackgroundStatesColor(sf::Color(253, 247, 216), sf::Color(255, 240, 180),
            sf::Color(250, 239, 250), sf::Color(253, 249, 221));
    }
    m_undoButton->initHighlightTextAlign(HighlightTextAlign::Down);
    m_redoButton->initHighlightTextAlign(HighlightTextAlign::Down);
    buttons.clear();
}

void EditionView::initWidgetsList()
{
    for (const auto& button : m_editionToolsButtons)
        m_widgets.push_back(button);
    m_widgets.push_back(m_undoButton);
    m_widgets.push_back(m_redoButton);
    m_widgets.push_back(m_toolsText);
    m_widgets.push_back(m_editionToolsBox);
}
