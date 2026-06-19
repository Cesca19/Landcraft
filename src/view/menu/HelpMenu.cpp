//
// Created by fran on 17/06/2026.
//

#include "HelpMenu.hpp"

HelpMenu::HelpMenu(const sf::Vector2f &windowSize)
    : m_isVisible(false)
    , m_cardOffset(6)
    , m_windowSize(windowSize)
{
    m_cardSize = {m_windowSize.x - m_cardOffset, m_windowSize.y - m_cardOffset};
    m_cardBox = UIFactory::createBox({0, 0}, m_cardSize);
    UIFactory::applyDefaultBoxStyle(m_cardBox);

    m_backButton = UIFactory::createSpriteButton(
        "assets/textures/ui/prev_64.png", {0, 0}, {32, 32}, "", 12);
    UIFactory::applyDefaultSpriteButtonStyle(m_backButton, HighlightTextAlign::Down, true);

    m_title = UIFactory::createText({0, 0}, "Help & Controls", 42);
    UIFactory::applyDefaultTextStyle(m_title, UIFactory::TextVariant::Title);

    m_subtitle = UIFactory::createText({0, 0},
        "Learn how to use Landcraft's tools", 18);
    UIFactory::applyDefaultTextStyle(m_subtitle, UIFactory::TextVariant::Default);

    m_navigationBox = UIFactory::createBox({0, 0}, {100, 100});
    m_editionBox = UIFactory::createBox({0, 0}, {100, 100});
    m_actionsBox = UIFactory::createBox({0, 0}, {100, 100});
    UIFactory::applyDefaultBoxStyle(m_navigationBox);
    UIFactory::applyDefaultBoxStyle(m_editionBox);
    UIFactory::applyDefaultBoxStyle(m_actionsBox);

    m_navigationHeader = UIFactory::createHelpSectionHeader(
        {0, 0}, "assets/textures/ui/nav_icon_48.png", "Navigation");
    m_editionHeader = UIFactory::createHelpSectionHeader(
        {0, 0}, "assets/textures/ui/tools_icon_48.png", "Editing Tools");
    m_actionsHeader = UIFactory::createHelpSectionHeader(
        {0, 0}, "assets/textures/ui/star_icon_48.png", "General Actions");

    initControlRows();

    m_tipBox = UIFactory::createBox({0, 0}, {100 , 100});
    m_tipBox->initColors(sf::Color(248, 246, 252), sf::Color(220, 210, 240));

    m_tipIcon = UIFactory::createImage(
        "assets/textures/ui/hint_64.png", {0, 0}, {28, 28});

    m_tipTitle = UIFactory::createText({0, 0}, "Tip", 22);
    UIFactory::applyDefaultTextStyle(m_tipTitle, UIFactory::TextVariant::Label);

    m_tipText = UIFactory::createText({0, 0},
        "When a UI element is selected, navigation keys "
        "(Tab, arrows, Space, Enter, Esc) \nare captured by the UI and no longer "
        "control the 3D view.",
        15);
    UIFactory::applyDefaultTextStyle(m_tipText, UIFactory::TextVariant::Default);

    m_tipImage = UIFactory::createImage("assets/textures/ui/menu_image.png", {0, 0});
    m_tipImage->setScale({0.35f, 0.35f});

    initWidgetsList();
    setMenuPosition(windowSize);
    setVisibility(false);
}

void HelpMenu::initControlRows()
{
    const sf::Vector2f rowSize(280, 56);

    m_navigationRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/pan_icon_48.png",
        "Pan camera", "Move around the scene",
        {{"assets/textures/ui/mouse_48.png", "Middle click + drag"}},
        {{"Z", "Q", "S", "D"}}));

    m_navigationRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/zoom_icon_48.png",
        "Zoom", "Mouse or keyboard",
        {{"assets/textures/ui/mouse_48.png", "Scroll wheel"}},
        {{"I", "P"}}));

    m_navigationRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/yaw_icon_64.png",
        "Orbit (yaw)", "Right click + drag or keyboard",
        {{"assets/textures/ui/mouse_48.png", "Right click + drag"}},
        {{"K", "M"}}));

    m_navigationRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/pitch_icon_64.png",
        "Tilt (pitch)", "Right click + drag or keyboard",
        {{"assets/textures/ui/mouse_48.png", "Right click + drag"}},
        {{"O", "L"}}));

    m_navigationRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/recenter_view_512.png",
        "Recenter view", "Return to the default view", {}, {{"R"}}));

    m_editionRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/switch_tool_icon_64.png",
        "Switch edition tool", "Switch between tools", {}, {{"T"}}));

    m_editionRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/toogle_selection_icon_64.png",
        "Toggle selection mode", "Tile / corner mode", {}, {{"Space"}}));

    m_editionRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/brush_size_512.png",
        "Brush size", "Increase / decrease the brush", {},
        {{" + ", " - "}}));

    m_editionRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/brush_shape_icon_64.png",
        "Cycle brush shape", "Change the brush shape", {},
        {{"A", "E"}}));

    m_editionRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/paint_palette_64.png",
        "Select paint texture", "Choose the paint texture", {},
        {{"0", "1", "2", "3", "4"}},
        {
            {sf::Color(210, 210, 210), "0 Clear"},
            {sf::Color(120, 180, 90), "1 Grass"},
            {sf::Color(210, 180, 120), "2 Sand"},
            {sf::Color(130, 130, 130), "3 Rock"},
            {sf::Color(210, 230, 245), "4 Snow"}
        }));

    m_editionRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/terrain_generator_512.png",
        "Generate terrain / cycle noise", "Generate terrain / noise type", {},
        {{"G", "N"}}));

    m_actionsRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/help_512.png",
        "Help & Controls", "Show or hide this help screen", {}, {{"H"}}));

    m_actionsRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/undo_512.png",
        "Undo / Redo", "Undo / redo an action", {},
        {{"Ctrl", "+", "Z"}, {"Ctrl", "+", "Y"}}));

    m_actionsRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/save_map_48.png",
        "Save map", "Save the map", {},
        {{"Ctrl", "+", "S"}}));

    m_actionsRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/open_map_512.png",
        "Load map", "Load a map", {},
        {{"Ctrl", "+", "O"}}));

    m_actionsRows.push_back(UIFactory::createHelpControlRow(
        {0, 0}, rowSize, "assets/textures/ui/exit_48.png",
        "Quit (with confirmation)", "Exit the editor", {}, {{"Esc"}}));
}

HelpMenu::~HelpMenu()
{
    for (IWidget *widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void HelpMenu::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
    for (IWidget *widget : m_widgets)
        widget->setVisibility(isVisible);
}

bool HelpMenu::isVisible() const
{
    return m_isVisible;
}

void HelpMenu::onWindowResized(const sf::Vector2f &windowSize)
{
    m_windowSize = windowSize;
    setMenuPosition(windowSize);
}

void HelpMenu::setCloseButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_backButton->initOnClickCallback(callback);
}

void HelpMenu::setMenuPosition(const sf::Vector2f &windowSize)
{
    m_cardSize = {windowSize.x - m_cardOffset, windowSize.y - m_cardOffset};
    m_cardPosition = {m_cardOffset / 2, m_cardOffset / 2};

    m_cardBox->setSize(m_cardSize);
    m_cardBox->setPosition(m_cardPosition);

    const float headerTop = m_cardPosition.y + 40;
    m_backButton->setPosition(m_cardPosition + sf::Vector2f(28, 28));

    const sf::FloatRect titleBounds = m_title->getBounds();
    m_title->setPosition({
        windowSize.x / 2 - titleBounds.width / 2,
        headerTop});

    const sf::FloatRect subtitleBounds = m_subtitle->getBounds();
    m_subtitle->setPosition({
        windowSize.x / 2 - subtitleBounds.width / 2,
        headerTop + titleBounds.height + 12});

    const float columnsTop = headerTop + titleBounds.height + subtitleBounds.height + 40;
    const float columnsHeight = m_cardSize.y * 0.52f;
    const float availableWidth = m_cardSize.x - s_columnSideMargin * 2;
    const float columnWidth = (availableWidth - s_columnGap * 2) / 3;

    const sf::Vector2f columnSize(columnWidth, columnsHeight);
    const sf::Vector2f navigationPosition(
        m_cardPosition.x + s_columnSideMargin, columnsTop);
    const sf::Vector2f editionPosition(
        navigationPosition.x + columnWidth + s_columnGap, columnsTop);
    const sf::Vector2f actionsPosition(
        editionPosition.x + columnWidth + s_columnGap, columnsTop);

    m_navigationBox->setSize(columnSize);
    m_navigationBox->setPosition(navigationPosition);
    m_editionBox->setSize(columnSize);
    m_editionBox->setPosition(editionPosition);
    m_actionsBox->setSize(columnSize);
    m_actionsBox->setPosition(actionsPosition);

    const sf::Vector2f rowSize(columnWidth - 24, 56);
    for (HelpControlRow *row : m_navigationRows)
        row->setSize(rowSize);
    for (HelpControlRow *row : m_editionRows)
        row->setSize(rowSize);
    for (HelpControlRow *row : m_actionsRows)
        row->setSize(rowSize);

    setColumnPosition(navigationPosition, columnSize, m_navigationHeader, m_navigationRows);
    setColumnPosition(editionPosition, columnSize, m_editionHeader, m_editionRows);
    setColumnPosition(actionsPosition, columnSize, m_actionsHeader, m_actionsRows);

    const float tipTop = columnsTop + columnsHeight + 28;
    const float tipHeight = m_cardPosition.y + m_cardSize.y - tipTop - 32;
    const float tipWidth = m_cardSize.x - s_columnSideMargin * 2;
    const sf::Vector2f tipPosition(m_cardPosition.x + s_columnSideMargin, tipTop);

    m_tipBox->setSize({tipWidth, tipHeight});
    m_tipBox->setPosition(tipPosition);

    m_tipIcon->setPosition(tipPosition + sf::Vector2f(24, 22));
    m_tipTitle->setPosition(tipPosition + sf::Vector2f(64, 24));
    m_tipText->setPosition(tipPosition + sf::Vector2f(64, 52));

    const sf::FloatRect tipImageBounds = m_tipImage->getBounds();
    m_tipImage->setPosition({
        tipPosition.x + tipWidth - tipImageBounds.width - 24,
        tipPosition.y + tipHeight / 2 - tipImageBounds.height / 2});
}

void HelpMenu::setColumnPosition(const sf::Vector2f &columnPosition,
    const sf::Vector2f &columnSize, HelpSectionHeader *header,
    const std::vector<HelpControlRow *> &rows) const
{
    header->setPosition(columnPosition + sf::Vector2f(200, 18));

    float rowTop = columnPosition.y + s_sectionHeaderHeight + 18;
    for (HelpControlRow *row : rows) {
        row->setPosition({columnPosition.x + 12, rowTop});
        rowTop += row->getRowHeight() + s_rowVerticalGap;
    }
}


void HelpMenu::initWidgetsList()
{
    m_widgets.push_back(m_cardBox);
    m_widgets.push_back(m_backButton);
    m_widgets.push_back(m_title);
    m_widgets.push_back(m_subtitle);
    m_widgets.push_back(m_navigationBox);
    m_widgets.push_back(m_editionBox);
    m_widgets.push_back(m_actionsBox);
    m_widgets.push_back(m_navigationHeader);
    m_widgets.push_back(m_editionHeader);
    m_widgets.push_back(m_actionsHeader);

    for (HelpControlRow *row : m_navigationRows)
        m_widgets.push_back(row);
    for (HelpControlRow *row : m_editionRows)
        m_widgets.push_back(row);
    for (HelpControlRow *row : m_actionsRows)
        m_widgets.push_back(row);

    m_widgets.push_back(m_tipBox);
    m_widgets.push_back(m_tipIcon);
    m_widgets.push_back(m_tipTitle);
    m_widgets.push_back(m_tipText);
    m_widgets.push_back(m_tipImage);
}
