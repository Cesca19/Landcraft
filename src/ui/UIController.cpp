//
// Created by fran on 08/04/2026.
//

#include "UIController.hpp"

UIController::UIController()
    : m_focusedWidget(nullptr)
    , m_hoveredWidget(nullptr)
    , m_onDestroy(nullptr)
{
}

UIController::~UIController()
{
    if (m_onDestroy != nullptr)
        m_onDestroy();
}

void UIController::addWidget(std::unique_ptr<IWidget> widget)
{
    m_widgets.push_back(std::move(widget));
}

void UIController::removeWidget(IWidget *widgetToRemove)
{
    if (m_focusedWidget == widgetToRemove)
        m_focusedWidget = nullptr;
    if (m_hoveredWidget == widgetToRemove)
        m_hoveredWidget = nullptr;

    m_widgets.erase(
        std::remove_if(m_widgets.begin(), m_widgets.end(),
            [widgetToRemove](const std::unique_ptr<IWidget>& widget) {
                return widget.get() == widgetToRemove;
            }),
        m_widgets.end()
    );
}

void UIController::handleEvents(const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseMoved)
        findHoveredWidget(window);
    if (event.type == sf::Event::MouseButtonPressed
        && (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)
        && m_hoveredWidget != nullptr)
        m_hoveredWidget->setState(WidgetState::Pressed);
    if (event.type == sf::Event::MouseButtonReleased
        && (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)
        && m_hoveredWidget != nullptr)
        m_hoveredWidget->setState(WidgetState::Hovered);
}

void UIController::handleContinuousEvents(float deltaTime, const sf::RenderWindow &window)
{
}

void UIController::update(const float deltaTime, sf::RenderWindow &window) const
{
    for (const auto& widget : m_widgets)
        if (widget->isVisible())
            widget->update(deltaTime);
}

void UIController::draw(sf::RenderWindow &window) const
{
    const sf::View lastView = window.getView();

    window.setView(window.getDefaultView());
    for (const auto& widget : m_widgets)
        if (widget->isVisible())
            widget->draw(window);
    window.setView(lastView);
}

void UIController::setOnDestroy(std::function<void()> onDestroy)
{
    m_onDestroy = std::move(onDestroy);
}

void UIController::findHoveredWidget(const sf::RenderWindow &window)
{
    const sf::Vector2f mousePos = getMouseScreenPosition(window);
    for (const auto& widget : m_widgets) {
        if (!widget->isVisible())
            continue;
        sf::FloatRect bounds = widget->getBounds();
        if (widget->isInteractable() && bounds.contains(mousePos))
        {
            if (m_hoveredWidget == widget.get())
                return;
            if (m_hoveredWidget != nullptr)
                m_hoveredWidget->setState(WidgetState::Base);
            if (m_focusedWidget != nullptr)
                m_focusedWidget->setState(WidgetState::Base);

            widget->setState(WidgetState::Hovered);
            m_hoveredWidget = widget.get();
            m_focusedWidget = nullptr;
            return;
        }
    }
    if (m_hoveredWidget != nullptr) {
        m_hoveredWidget->setState(WidgetState::Base);
        m_hoveredWidget = nullptr;
    }
}

sf::Vector2f UIController::getMouseScreenPosition(const sf::RenderWindow &window)
{
    // get the current mouse position in the window
    const sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    // later use the created ui view
    return window.mapPixelToCoords(pixelPos, window.getDefaultView());
}
