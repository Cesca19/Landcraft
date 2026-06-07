//
// Created by fran on 10/04/2026.
//

#ifndef LANDCRAFT_TEXT_HPP
#define LANDCRAFT_TEXT_HPP

#include "IWidget.hpp"

namespace sf {
    class RenderWindow;
}

class Text : public IWidget {
public:
    Text(const sf::Vector2f position, const std::string& content, int characterSize);
    void init(sf::Color color, sf::Uint32 style);
    void setContent(const std::string& content);
    
    bool isInteractable() const override;

    bool isVisible() const override;
    void setVisibility(bool isVisible) override;

    bool isSelected() const override;
    void setSelected(bool isSelected) override;

    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void setState(WidgetState state) override;
    void handleKeyBoardEvents(const sf::Event& event) override;
    void setPosition(const sf::Vector2f& position) override;
    bool shouldFocusOnClick() const override;
protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;
private:
    sf::Text m_text;
    bool m_isVisible;
};



#endif //LANDCRAFT_TEXT_HPP
