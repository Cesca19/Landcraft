//
// Created by fran on 17/06/2026.
//

#include "StartMenu.hpp"

StartMenu::StartMenu(const sf::Vector2f &windowSize)
    : m_isVisible(true)
    , m_windowSize(windowSize)
    , m_previewScale(1)
    , m_actionButtonHeight(80)
    , m_cardOffset(6)
    , m_actionButtonWidth(400)
{
    m_cardSize = {m_windowSize.x - m_cardOffset, m_windowSize.y - m_cardOffset};
    m_cardBox = UIFactory::createBox({0, 0}, m_cardSize);
    UIFactory::applyDefaultBoxStyle(m_cardBox);

    m_brandTitle = UIFactory::createText({0, 0}, "LANDCRAFT", 65);
    UIFactory::applyDefaultTextStyle(m_brandTitle, UIFactory::TextVariant::Value);

    m_projectDescription = UIFactory::createText({0, 0},
        "A vast isometric world to build, sculpt, and customize according to your vision.",
        17);
    UIFactory::applyDefaultTextStyle(m_projectDescription, UIFactory::TextVariant::Default);

    m_newProjectButton = UIFactory::createMenuActionButton({0, 0}, {m_actionButtonWidth, m_actionButtonHeight},
        "assets/textures/ui/new_project_512.png", "New Project", "Create a new empty map");
    m_loadMapButton = UIFactory::createMenuActionButton({0, 0}, {m_actionButtonWidth, m_actionButtonHeight},
        "assets/textures/ui/open_map_512.png", "Open Map", "Load an existing map");
    m_helpButton = UIFactory::createMenuActionButton({0, 0}, {m_actionButtonWidth, m_actionButtonHeight},
        "assets/textures/ui/help_512.png", "Help & Controls",
        "Learn how to use Landcraft");

    m_previewImage = UIFactory::createImage("assets/textures/ui/menu_image.png", {0, 0});
    m_previewImage->setScale({0.6, 0.6});

    initWidgetsList();
    setMenuPosition(windowSize);
}

StartMenu::~StartMenu()
{
    for (IWidget *widget : m_widgets)
        UIFactory::removeWidget(widget);
    m_widgets.clear();
}

void StartMenu::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
    for (IWidget *widget : m_widgets)
        widget->setVisibility(isVisible);
}

bool StartMenu::isVisible() const
{
    return m_isVisible;
}

void StartMenu::onWindowResized(const sf::Vector2f &windowSize)
{
    m_windowSize = windowSize;
    setMenuPosition(windowSize);
}

void StartMenu::setNewProjectButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_newProjectButton->initOnClickCallback(callback);
}

void StartMenu::setLoadMapButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_loadMapButton->initOnClickCallback(callback);
}

void StartMenu::setHelpButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_helpButton->initOnClickCallback(callback);
}

void StartMenu::setMenuPosition(const sf::Vector2f &windowSize)
{
    constexpr float actionButtonSpacing = 35;
    float startXOffset = 500;

    m_cardPosition = {m_cardOffset / 2 , m_cardOffset / 2};
    m_cardBox->setPosition(m_cardPosition);

    m_cardPosition = m_cardPosition + sf::Vector2f(startXOffset, 100);
    m_brandTitle->setPosition(m_cardPosition + sf::Vector2f(250, 25));

    m_projectDescription->setPosition(m_cardPosition + sf::Vector2f(125, 125));

    m_cardPosition = m_cardPosition + sf::Vector2f(- startXOffset / 2, 250);
    const float buttonWidth = m_cardSize.x - 84;
    float buttonY = m_cardPosition.y;
    float buttonX = windowSize.x / 2.f + 150;

    m_previewImage->setPosition(sf::Vector2f(150, windowSize.y / 2.f) +
        sf::Vector2f(0, (- m_previewImage->getBounds().height / 2.f) + 25));

    m_newProjectButton->setPosition({buttonX, buttonY});
    buttonY += m_actionButtonHeight + actionButtonSpacing;

    m_loadMapButton->setPosition({buttonX, buttonY});
    buttonY += m_actionButtonHeight + actionButtonSpacing;

    m_helpButton->setPosition({buttonX, buttonY});

    m_previewCenter = {
        m_cardPosition.x + m_cardSize.x + (windowSize.x - m_cardPosition.x - m_cardSize.x) / 2,
        windowSize.y * 0.52f
    };
    m_previewScale = std::min(windowSize.x, windowSize.y) / 1500;
}

void StartMenu::initWidgetsList()
{
    m_widgets.push_back(m_cardBox);
    m_widgets.push_back(m_brandTitle);
    m_widgets.push_back(m_projectDescription);
    m_widgets.push_back(m_newProjectButton);
    m_widgets.push_back(m_loadMapButton);
    m_widgets.push_back(m_helpButton);
    m_widgets.push_back(m_previewImage);
}
