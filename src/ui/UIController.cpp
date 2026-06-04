//
// Created by fran on 08/04/2026.
//

#include "UIController.hpp"


UIController::UIController()
    : m_isMouseHoverUI(false)
    , m_onDestroy(nullptr)
    , m_focusedWidget(nullptr)
    , m_hoveredWidget(nullptr)
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
    handleMouseEvents(event, window);
    handleKeyBoardEvents(event);
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

bool UIController::isUserOverUI() const
{
    return isMouseHoverUI() || isKeyBoardNavigatingHoverUI();
}

bool UIController::isMouseHoverUI() const
{
    // isMouseOverUI takes into account non interactable widgets
    return m_hoveredWidget != nullptr || m_isMouseHoverUI;
}

bool UIController::isKeyBoardNavigatingHoverUI() const
{
    return m_focusedWidget != nullptr;
}

bool UIController::shouldForwardEventToWorld(const sf::Event &event) const
{
    const bool isMouseEvent = event.type == sf::Event::MouseMoved
        || event.type == sf::Event::MouseButtonPressed
        || event.type == sf::Event::MouseButtonReleased
        || event.type == sf::Event::MouseWheelScrolled;
    const bool isKeyboardEvent = event.type == sf::Event::KeyPressed
        || event.type == sf::Event::KeyReleased;

    if (isMouseEvent)
        return !isMouseHoverUI() || event.type == sf::Event::MouseButtonReleased;

    if (isKeyboardEvent && isKeyBoardNavigatingHoverUI()) {
        switch (event.key.code) {
            case sf::Keyboard::Escape:
            case sf::Keyboard::Tab:
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
            case sf::Keyboard::Space:
            case sf::Keyboard::Enter:
                return false;
            default:
                break;
        }
    }

    return true;
}

void UIController::handleMouseEvents(const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseMoved)
        findHoveredWidget(window);
    if (event.type == sf::Event::MouseButtonPressed) {
        if ((event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)
        && m_hoveredWidget != nullptr) {
            // Mouse interaction should not keep keyboard navigation focus active.
            if (m_focusedWidget != nullptr)
                unfocusCurrentWidget();
            if (m_hoveredWidget->isInteractable() && m_hoveredWidget->shouldFocusOnClick()) {
                m_focusedWidget = m_hoveredWidget;
                m_focusedWidget->setState(WidgetState::Focused);
                m_hoveredWidget = nullptr;
                return;
            }
            
            m_hoveredWidget->setState(WidgetState::Pressed);
            
            m_isMouseHoverUI = true;
        } else if (m_hoveredWidget == nullptr)
            unfocusCurrentWidget();
    }
    if (event.type == sf::Event::MouseButtonReleased
        && (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right)
        && m_hoveredWidget != nullptr)
        m_hoveredWidget->setState(WidgetState::Hovered);
}

void UIController::handleKeyBoardEvents(const sf::Event &event)
{
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Escape:
                unfocusCurrentWidget();
                break;
            case sf::Keyboard::Tab:
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
                spatialNavigation(event.key.code);
                break;
            case sf::Keyboard::Space:
            case sf::Keyboard::Enter:
                if (m_focusedWidget)
                    m_focusedWidget->setState(WidgetState::Pressed);
                break;
            default:
                break;
        }
    }
    if (event.type == sf::Event::KeyReleased
        && (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Enter)
        && m_focusedWidget != nullptr)
            m_focusedWidget->setState(WidgetState::Focused);
    for (const auto& widget : m_widgets)
        if (widget->isVisible())
            widget->handleKeyBoardEvents(event);

}

void UIController::findHoveredWidget(const sf::RenderWindow &window)
{
    m_isMouseHoverUI = false;
    const sf::Vector2f mousePos = getMouseScreenPosition(window);
    for (const auto& widget : m_widgets) {
        if (!widget->isVisible())
            continue;
        sf::FloatRect bounds = widget->getBounds();
        if (bounds.contains(mousePos))
            m_isMouseHoverUI = true;
        if (widget->isInteractable() && bounds.contains(mousePos))
        {
            if (m_hoveredWidget == widget.get())
                return;
            if (m_hoveredWidget != nullptr)
                m_hoveredWidget->setState(WidgetState::Base);
            if (m_focusedWidget != nullptr)
                unfocusCurrentWidget();

            widget->setState(WidgetState::Hovered);
            m_hoveredWidget = widget.get();
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

void UIController::spatialNavigation(const sf::Keyboard::Key key)
{
    if (m_hoveredWidget) {
        m_hoveredWidget->setState(WidgetState::Base);
        m_focusedWidget = m_hoveredWidget;
        m_focusedWidget->setState(WidgetState::Focused);
        m_hoveredWidget = nullptr;
    }

    if (!m_focusedWidget) {
        focusOnDefaultWidget();
        return;
    }

    sf::Vector2f mainAxis(0.f, 0.f);
    sf::Vector2f orthoAxis(0.f, 0.f);
    const bool isShiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
                            || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
    switch (key) {
        case sf::Keyboard::Tab:
            navigateSequentially(!isShiftPressed);
            break;
        case sf::Keyboard::Right:
            mainAxis = {1.f, 0.f};
            orthoAxis = {0.f, 1.f};
            break;
        case sf::Keyboard::Left:
            mainAxis = {-1.f, 0.f};
            orthoAxis = {0.f, 1.f};
            break;
        case sf::Keyboard::Down:
            mainAxis = {0.f, 1.f};
            orthoAxis = {1.f, 0.f};
            break;
        case sf::Keyboard::Up:
            mainAxis = {0.f, -1.f};
            orthoAxis = {1.f, 0.f};
            break;
        default:
            return;
    }

    IWidget* nextWidget = findClosestWidgetAlongAxis(mainAxis, orthoAxis, m_focusedWidget->getCenter());
    if (nextWidget) {
        unfocusCurrentWidget();
        m_focusedWidget = nextWidget;
        m_focusedWidget->setState(WidgetState::Focused);
    }
}
IWidget* UIController::findClosestWidgetAlongAxis(const sf::Vector2f mainAxis, const sf::Vector2f orthogonalAxis, const sf::Vector2f widgetCenter) const
{
    // up : main (0, 1)  | down : main (0,-1) | left (-1,0) | right (1,0)
    IWidget* bestCandidate = nullptr;
    float bestScore = std::numeric_limits<float>::max();

    for (const auto& widget : m_widgets) {
        constexpr float orthogonalWeight = 5.0f;
        IWidget* candidate = widget.get();
        // Ignore the currently focused widget, hidden ones, or non-interactable ones
        if (candidate == m_focusedWidget || !candidate->isVisible() || !candidate->isInteractable())
            continue;
        sf::Vector2f candidateCenter = candidate->getCenter();
        // Vector representing the path from current to candidate widget
        sf::Vector2f delta = candidateCenter - widgetCenter;
        // axial distance
        const float axialDist = MathUtils::dotProduct(delta, mainAxis);
        // If delta and main axis have a positive dot product that mean there are in the same direction
        if (axialDist <= 0.0f)
            continue;
        // orthogonal distance (Misalignment)
        const float orthoDist = std::abs(MathUtils::dotProduct(delta, orthogonalAxis));

        const float score = axialDist + (orthoDist * orthogonalWeight);
        if (score < bestScore) {
            bestScore = score;
            bestCandidate = candidate;
        }
    }
    return bestCandidate;
}

void UIController::navigateSequentially(const bool forward)
{
    std::vector<IWidget*> focusableWidgets;
    for (const auto& widget : m_widgets) {
        if (widget->isVisible() && widget->isInteractable()) {
            focusableWidgets.push_back(widget.get());
        }
    }
    if (focusableWidgets.empty())
        return;

    // sort the widgets in "Reading Order" (Top-to-Bottom, Left-to-Right)
    std::sort(focusableWidgets.begin(), focusableWidgets.end(), [](const IWidget* a, const IWidget* b) {
        const sf::Vector2f centerA = a->getCenter();
        const sf::Vector2f centerB = b->getCenter();
        // Tolerance in pixels to consider widgets as being on the "same row"
        constexpr float rowTolerance = 15.0f;
        // If the difference in Y is significant, the one higher up (smaller Y) comes first
        if (std::abs(centerA.y - centerB.y) > rowTolerance) {
            return centerA.y < centerB.y;
        }
        // Otherwise, they are on the same visual row, sort from Left to Right (smaller X first)
        return centerA.x < centerB.x;
    });

    // Find the currently focused widget's index in this sorted list
    int currentIndex = -1;
    if (m_focusedWidget != nullptr) {
        auto it = std::find(focusableWidgets.begin(), focusableWidgets.end(), m_focusedWidget);
        if (it != focusableWidgets.end()) {
            currentIndex = std::distance(focusableWidgets.begin(), it);
        }
    }

    // find the next index (wrap around if necessary)
    const int step = forward ? 1 : -1;
    int nextIndex;
    if (currentIndex == -1) {
        // If nothing is focused, start at the beginning (or end if going backwards)
        nextIndex = forward ? 0 : focusableWidgets.size() - 1;
    } else {
        nextIndex = (currentIndex + step) % static_cast<int>(focusableWidgets.size());
        if (nextIndex < 0)
            nextIndex += focusableWidgets.size(); // Handle negative modulo for backward navigation
    }

    unfocusCurrentWidget();
    m_focusedWidget = focusableWidgets[nextIndex];
    m_focusedWidget->setState(WidgetState::Focused);
}
void UIController::focusOnDefaultWidget()
{
    IWidget *defaultWidget = findClosestWidgetAlongAxis({1.f, 0.f}, {0.f, 1.f}, {0,0});
    if (!defaultWidget)
        return;
    m_focusedWidget = defaultWidget;
    m_focusedWidget->setState(WidgetState::Focused);
}

void UIController::unfocusCurrentWidget()
{
    if (m_focusedWidget == nullptr)
        return;
    m_focusedWidget->setState(WidgetState::Base);
    m_focusedWidget = nullptr;
}
