//
// Created by fran on 17/06/2026.
//

#ifndef LANDCRAFT_HELPCONTROLROW_HPP
#define LANDCRAFT_HELPCONTROLROW_HPP

#include <string>
#include <vector>
#include "IWidget.hpp"
#include "Keycap.hpp"

class HelpControlRow : public IWidget {
public:
    struct LegendItem {
        sf::Color color;
        std::string label;
    };

    struct MouseHint {
        std::string iconPath;
        std::string label;
    };

    HelpControlRow(const sf::Vector2f &position, const sf::Vector2f &size,
        const std::string &iconPath, const std::string &title,
        const std::string &subtitle,
        const std::vector<MouseHint> &mouseHints = {},
        const std::vector<std::vector<std::string>> &shortcutGroups = {},
        const std::vector<LegendItem> &legendItems = {});

    bool isInteractable() const override;
    bool isVisible() const override;
    void setVisibility(bool isVisible) override;
    bool isSelected() const override;
    void setSelected(bool isSelected) override;
    sf::FloatRect getBounds() const override;
    sf::Vector2f getCenter() const override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow &window) const override;
    void setState(WidgetState state) override;
    void handleKeyBoardEvents(const sf::Event &event) override;
    bool shouldFocusOnClick() const override;
    void setPosition(const sf::Vector2f &position) override;
    void setSize(const sf::Vector2f &size);
    float getRowHeight() const;

protected:
    void onBase() override;
    void onHover() override;
    void onFocus() override;
    void onPress() override;

private:
    struct MouseHintVisual {
        sf::Sprite icon;
        sf::Text label;
    };

    enum class ShortcutPartType { Keycap, Separator };
    struct ShortcutPart {
        ShortcutPartType type;
        std::size_t index;
    };

    void buildShortcuts(const std::vector<MouseHint> &mouseHints,
        const std::vector<std::vector<std::string>> &shortcutGroups);
    void buildLegend(const std::vector<LegendItem> &legendItems);
    void updateRowHeight();
    void refreshLayout();
    void layoutMouseHints(float rowY);
    void layoutKeyboardShortcuts(float rowY);
    void layoutLegend(float startY);

    static constexpr float s_iconSize = 22.f;
    static constexpr float s_textLeftPadding = 34.f;
    static constexpr float s_baseRowHeight = 56.f;
    static constexpr float s_mouseHintRowHeight = 22.f;
    static constexpr float s_shortcutRowGap = 6.f;
    static constexpr float s_legendRowHeight = 24.f;
    static constexpr float s_shortcutIconSize = 20.f;
    static constexpr float s_mouseHintIconTextGap = 6.f;
    static constexpr float s_shortcutGap = 6.f;
    static constexpr float s_groupGap = 12.f;
    static constexpr float s_legendDotRadius = 5.f;
    static constexpr float s_shortcutsRightPadding = 12.f;

    bool m_isVisible;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    sf::Sprite m_iconSprite;
    sf::Text m_title;
    sf::Text m_subtitle;
    std::vector<MouseHintVisual> m_mouseHints;
    std::vector<Keycap> m_keycaps;
    std::vector<sf::Text> m_shortcutSeparators;
    std::vector<ShortcutPart> m_shortcutParts;
    std::vector<sf::CircleShape> m_legendDots;
    std::vector<sf::Text> m_legendLabels;
    std::vector<std::size_t> m_groupEndKeycapIndices;
    float m_rowHeight;
};

#endif //LANDCRAFT_HELPCONTROLROW_HPP
