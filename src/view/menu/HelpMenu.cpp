//
// Created by fran on 17/06/2026.
//

#include "HelpMenu.hpp"

HelpMenu::HelpMenu(const sf::Vector2f &windowSize)
    : m_isVisible(false)
    , m_windowSize(windowSize)
{
    const char *controlsText =
            "Navigation\n"
            "  Z / Q / S / D  -  Pan camera\n"
            "  Mouse wheel  -  Zoom at cursor\n"
            "  I / P  -  Zoom in / out\n"
            "  K / M  -  Rotate yaw\n"
            "  O / L  -  Rotate pitch\n"
            "  R  -  Recenter view\n"
            "  Middle mouse  -  Pan (drag)\n"
            "\n"
            "Edition\n"
            "  T  -  Switch tool (Elevation / Paint)\n"
            "  Ctrl + Z  -  Undo\n"
            "  Ctrl + Y  -  Redo\n"
            "  Ctrl + S  -  Save map\n"
            "  Ctrl + O  -  Open map\n"
            "\n"
            "General\n"
            "  Escape  -  Quit menu (in editor)";
    m_menuBox = UIFactory::createBox({0, 0}, windowSize - sf::Vector2f(100, 100));
    UIFactory::applyDefaultBoxStyle(m_menuBox);

    m_title = UIFactory::createText({0, 0}, "Controls", 24);
    UIFactory::applyDefaultTextStyle(m_title, UIFactory::TextVariant::Title);

    m_controlsText = UIFactory::createText({0, 0}, std::string(controlsText), 14);
    UIFactory::applyDefaultTextStyle(m_controlsText, UIFactory::TextVariant::Default);

    m_closeButton = UIFactory::createTextButton({0, 0}, "Close", 17);
    UIFactory::applyDefaultTextButtonStyle(m_closeButton, UIFactory::TextVariant::Default);

    initWidgetsList();
    setPosition(windowSize);
    setVisibility(false);
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
    setPosition(windowSize);
}

void HelpMenu::setCloseButtonOnClickCallback(const std::function<void()> &callback) const
{
    m_closeButton->initOnClickCallback(callback);
}

void HelpMenu::setPosition(const sf::Vector2f &windowSize)
{
    const sf::Vector2f menuPosition = sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f)
        - sf::Vector2f(m_menuBox->getBounds().width / 2.f,  m_menuBox->getBounds().height / 2.f + 20);

    m_menuBox->setPosition(menuPosition);
    m_title->setPosition(menuPosition + sf::Vector2f(m_title->getBounds().width / 2.f, 20));
    m_controlsText->setPosition(menuPosition + sf::Vector2f(m_controlsText->getBounds().width / 2.f, 65));
    m_closeButton->setPosition(menuPosition + sf::Vector2f(m_closeButton->getBounds().width / 2.f, 370));
}

void HelpMenu::initWidgetsList()
{
    m_widgets.push_back(m_menuBox);
    m_widgets.push_back(m_title);
    m_widgets.push_back(m_controlsText);
    m_widgets.push_back(m_closeButton);
}
