//
// Created by fran on 17/03/2026.
//

#include "EditionController.hpp"

EditionController::EditionController(WorldModel &model, WorldView &view, const sf::Vector2f globalUIPosition)
    : m_currentEditionTool(-1)
    , m_editionToolsBoxPosition(0, 84)
    , m_globalUIStartPosition(globalUIPosition)
    , m_toolsStartMenuPosition(m_editionToolsBoxPosition + sf::Vector2f(90, 0))
{
    m_undoButton = UIFactory::createSpriteButton("assets/textures/ui/undo_512.png",
        globalUIPosition + sf::Vector2f(15, 16), {28, 28}, "Undo", 12);
    m_redoButton = UIFactory::createSpriteButton("assets/textures/ui/redo_512.png",
        globalUIPosition + sf::Vector2f(85, 16), {28, 28}, "Redo", 12);
    m_undoButton->initOnClickCallback([this, &model, &view] () {
        m_commandHistory.undoCommand(model, view);
    });
    m_redoButton->initOnClickCallback([this, &model, &view] () {
        m_commandHistory.redoCommand(model, view);
    });
    m_editionToolsBox = UIFactory::createBox(m_editionToolsBoxPosition, {85, 210});
    m_editionToolsBox->initColors(sf::Color(205, 185, 220), sf::Color(255, 255, 255));
    m_toolsText = UIFactory::createText(sf::Vector2f(10, 90), "Tools:", 20);
    m_toolsText->init(sf::Color(123, 101, 81), sf::Text::Bold | sf::Text::Underlined);
    const sf::Vector2f toolStartPos(15, 140);
    SpriteButton *elevationButton = UIFactory::createSpriteButton("assets/textures/ui/elevation_tool_512.png", toolStartPos, {32, 32}, "Elevation", 15);
    SpriteButton *paintButton = UIFactory::createSpriteButton("assets/textures/ui/paint_palette_64.png", toolStartPos + sf::Vector2f(0, 80), {32, 32}, "Paint", 15);
    m_editionToolsButtons.emplace_back(elevationButton);
    m_editionToolsButtons.emplace_back(paintButton);
    for (int i = 0; i < m_editionToolsButtons.size(); i++ ) {
        m_editionToolsButtons[i]->initOnClickCallback([this, i] () {
            this->selectEditionTool(i);
        });
    }
    applyUIStyle();
    initWidgetsList();

    m_editionTools.emplace_back(std::make_unique<ElevationTool>(m_toolsStartMenuPosition));
    m_editionTools.emplace_back(std::make_unique<PaintTool>(m_toolsStartMenuPosition));
    selectEditionTool(0);
}

EditionController::~EditionController()
{
    for (const auto widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void EditionController::handleEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view, SelectionController &selectionController)
{
    handleEditionToolSwitchEvents(event);
    m_editionTools[m_currentEditionTool]->handleEvents(window, event, model, view, selectionController, m_commandHistory);
    handleUndoRedoEvents(window, event, model, view);
}

void EditionController::handleContinuousEvents(const sf::RenderWindow &window, WorldModel &model, WorldView &view, SelectionController &selectionController)
{
    m_editionTools[m_currentEditionTool]->handleContinuousEvents(window, model, view, selectionController, m_commandHistory);
}

bool EditionController::isEditing() const
{
    return m_editionTools[m_currentEditionTool]->isEditing();
}

bool EditionController::isSelectionLocked() const
{
    return m_editionTools[m_currentEditionTool]->isSelectionLocked();
}

SelectionMode EditionController::getSelectionMode() const
{
    return m_editionTools[m_currentEditionTool]->getRequiredSelectionMode();
}

void EditionController::handleUndoRedoEvents(sf::RenderWindow &window, const sf::Event &event, WorldModel &model, WorldView &view)
{
    const bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
    if (isCtrlPressed && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
        m_commandHistory.undoCommand(model, view);
    if (isCtrlPressed && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Y)
        m_commandHistory.redoCommand(model, view);
}

void EditionController::handleEditionToolSwitchEvents(const sf::Event &event)
{
    // keyboard
    if (!isEditing() && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::T) {
        int nextTool = m_currentEditionTool + 1;
        if (nextTool > m_editionTools.size() - 1)
            nextTool = 0;
        selectEditionTool(nextTool);
    }
}

void EditionController::selectEditionTool(const int toolId)
{
    if (toolId == m_currentEditionTool)
        return;
    if (toolId > m_editionTools.size() - 1 || toolId < 0)
        std::cout << "Invalid ToolID" << std::endl;
    if (m_currentEditionTool != -1) {
        m_editionToolsButtons[m_currentEditionTool]->setSelected(false);
        m_editionTools[m_currentEditionTool]->onToolUnSelected();
    }
    m_currentEditionTool = toolId;
    m_editionToolsButtons[m_currentEditionTool]->setSelected(true);
    m_editionTools[m_currentEditionTool]->onToolSelected();
}

void EditionController::applyUIStyle()
{
    std::vector<SpriteButton*> buttons = { m_undoButton, m_redoButton };
    buttons.insert(buttons.end(), m_editionToolsButtons.begin(), m_editionToolsButtons.end());
    for (SpriteButton* button : buttons) {
        button->initOutlineStatesColors(sf::Color(255, 255, 255, 175), sf::Color::White,
            sf::Color::Cyan, sf::Color(255, 255, 255, 225), sf::Color(123, 101, 81));
        button->initBackgroundStatesColor(sf::Color(253, 247, 216), sf::Color(255, 240, 180),
            sf::Color(250, 239, 250), sf::Color(253, 249, 221));
    }
    m_undoButton->initHighlightTextAlign(HighlightTextAlign::Down);
    m_redoButton->initHighlightTextAlign(HighlightTextAlign::Down);
    buttons.clear();
}

void EditionController::initWidgetsList()
{
    for (const auto& button : m_editionToolsButtons)
        m_widgets.push_back(button);
    m_widgets.push_back(m_undoButton);
    m_widgets.push_back(m_redoButton);
    m_widgets.push_back(m_toolsText);
    m_widgets.push_back(m_editionToolsBox);
}
