//
// Created by fran on 17/06/2026.
//

#include "HelpControlRow.hpp"
#include <algorithm>

HelpControlRow::HelpControlRow(const sf::Vector2f &position, const sf::Vector2f &size,
    const std::string &iconPath, const std::string &title, const std::string &subtitle,
    const std::vector<MouseHint> &mouseHints,
    const std::vector<std::vector<std::string>> &shortcutGroups,
    const std::vector<LegendItem> &legendItems)
    : m_isVisible(true)
    , m_position(position)
    , m_size(size)
    , m_rowHeight(s_baseRowHeight)
{
    m_iconSprite.setTexture(ResourceManager::getInstance().getTexture(iconPath));
    const sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    m_iconSprite.setOrigin(iconLocal.width / 2.f, iconLocal.height / 2.f);
    m_iconSprite.setColor(sf::Color(110, 95, 150));

    m_title.setFont(ResourceManager::getInstance().getFont(
        "assets/fonts/ShadowsIntoLightTwo-Regular.ttf"));
    m_title.setString(title);
    m_title.setCharacterSize(17);
    m_title.setStyle(sf::Text::Bold);
    m_title.setFillColor(sf::Color(100, 80, 150));

    m_subtitle.setFont(ResourceManager::getInstance().getFont(
        "assets/fonts/ComicRelief-Regular.ttf"));
    m_subtitle.setString(subtitle);
    m_subtitle.setCharacterSize(15);
    m_subtitle.setFillColor(sf::Color(140, 130, 175));

    buildShortcuts(mouseHints, shortcutGroups);
    buildLegend(legendItems);
    updateRowHeight();
    refreshLayout();
}

void HelpControlRow::buildShortcuts(const std::vector<MouseHint> &mouseHints,
    const std::vector<std::vector<std::string>> &shortcutGroups)
{
    m_mouseHints.clear();
    m_keycaps.clear();
    m_shortcutSeparators.clear();
    m_shortcutParts.clear();
    m_groupEndKeycapIndices.clear();

    const sf::Font &labelFont = ResourceManager::getInstance().getFont(
        "assets/fonts/ComicRelief-Regular.ttf");

    for (const MouseHint &mouseHint : mouseHints) {
        MouseHintVisual visual;
        visual.icon.setTexture(
            ResourceManager::getInstance().getTexture(mouseHint.iconPath));
        const sf::FloatRect iconLocal = visual.icon.getLocalBounds();
        visual.icon.setOrigin(iconLocal.width / 2.f, iconLocal.height / 2.f);
        visual.icon.setColor(sf::Color(110, 95, 150));

        visual.label.setFont(labelFont);
        visual.label.setString(mouseHint.label);
        visual.label.setCharacterSize(12);
        visual.label.setFillColor(sf::Color(100, 80, 150));

        m_mouseHints.push_back(visual);
    }

    const sf::Font &separatorFont = ResourceManager::getInstance().getFont(
        "assets/fonts/ComicRelief-Regular.ttf");

    for (std::size_t groupIndex = 0; groupIndex < shortcutGroups.size(); ++groupIndex) {
        const std::vector<std::string> &group = shortcutGroups[groupIndex];
        for (const std::string &item : group) {
            if (item == "+") {
                sf::Text separator;
                separator.setFont(separatorFont);
                separator.setString("+");
                separator.setCharacterSize(13);
                separator.setFillColor(sf::Color(140, 130, 175));
                m_shortcutSeparators.push_back(separator);
                m_shortcutParts.push_back(
                    {ShortcutPartType::Separator, m_shortcutSeparators.size() - 1});
            } else {
                m_keycaps.emplace_back(sf::Vector2f(0.f, 0.f), item, 12);
                m_keycaps.back().initColors(
                    sf::Color::White,
                    sf::Color(200, 195, 210),
                    sf::Color(100, 80, 150));
                m_shortcutParts.push_back(
                    {ShortcutPartType::Keycap, m_keycaps.size() - 1});
            }
        }

        if (groupIndex + 1 < shortcutGroups.size() && !m_keycaps.empty())
            m_groupEndKeycapIndices.push_back(m_keycaps.size() - 1);
    }
}

void HelpControlRow::buildLegend(const std::vector<LegendItem> &legendItems)
{
    m_legendDots.clear();
    m_legendLabels.clear();

    const sf::Font &labelFont = ResourceManager::getInstance().getFont(
        "assets/fonts/ComicRelief-Regular.ttf");

    for (const LegendItem &item : legendItems) {
        sf::CircleShape dot(s_legendDotRadius);
        dot.setFillColor(item.color);
        dot.setOutlineColor(sf::Color(180, 170, 200));
        dot.setOutlineThickness(1.f);
        dot.setOrigin(s_legendDotRadius, s_legendDotRadius);
        m_legendDots.push_back(dot);

        sf::Text label;
        label.setFont(labelFont);
        label.setString(item.label);
        label.setCharacterSize(13);
        label.setFillColor(sf::Color(120, 110, 165));
        m_legendLabels.push_back(label);
    }
}

void HelpControlRow::updateRowHeight()
{
    m_rowHeight = s_baseRowHeight;
    if (!m_mouseHints.empty())
        m_rowHeight += s_mouseHintRowHeight + s_shortcutRowGap;
    if (!m_legendLabels.empty())
        m_rowHeight += s_legendRowHeight;
}

bool HelpControlRow::isInteractable() const
{
    return false;
}

bool HelpControlRow::isVisible() const
{
    return m_isVisible;
}

void HelpControlRow::setVisibility(const bool isVisible)
{
    m_isVisible = isVisible;
    for (Keycap &keycap : m_keycaps)
        keycap.setVisibility(isVisible);
}

bool HelpControlRow::isSelected() const
{
    return false;
}

void HelpControlRow::setSelected(bool isSelected)
{
}

sf::FloatRect HelpControlRow::getBounds() const
{
    return {m_position.x, m_position.y, m_size.x, m_rowHeight};
}

sf::Vector2f HelpControlRow::getCenter() const
{
    return {m_position.x + m_size.x / 2.f, m_position.y + m_rowHeight / 2.f};
}

void HelpControlRow::update(float deltaTime)
{
}

void HelpControlRow::draw(sf::RenderWindow &window) const
{
    if (!m_isVisible)
        return;

    window.draw(m_iconSprite);
    window.draw(m_title);
    window.draw(m_subtitle);

    for (const MouseHintVisual &mouseHint : m_mouseHints) {
        window.draw(mouseHint.icon);
        window.draw(mouseHint.label);
    }
    for (const sf::Text &separator : m_shortcutSeparators)
        window.draw(separator);
    for (const Keycap &keycap : m_keycaps)
        keycap.draw(window);

    for (const sf::CircleShape &dot : m_legendDots)
        window.draw(dot);
    for (const sf::Text &label : m_legendLabels)
        window.draw(label);
}

void HelpControlRow::setState(const WidgetState state)
{
}

void HelpControlRow::handleKeyBoardEvents(const sf::Event &event)
{
}

bool HelpControlRow::shouldFocusOnClick() const
{
    return false;
}

void HelpControlRow::setPosition(const sf::Vector2f &position)
{
    m_position = position;
    refreshLayout();
}

void HelpControlRow::setSize(const sf::Vector2f &size)
{
    m_size = size;
    refreshLayout();
}

float HelpControlRow::getRowHeight() const
{
    return m_rowHeight;
}

void HelpControlRow::layoutMouseHints(const float rowY)
{
    float shortcutX = m_position.x + m_size.x - s_shortcutsRightPadding;

    for (int hintIndex = static_cast<int>(m_mouseHints.size()) - 1;
        hintIndex >= 0; --hintIndex) {
        MouseHintVisual &mouseHint = m_mouseHints[hintIndex];
        const sf::FloatRect labelBounds = mouseHint.label.getLocalBounds();
        shortcutX -= labelBounds.width;
        mouseHint.label.setPosition(
            shortcutX - labelBounds.left,
            rowY + s_mouseHintRowHeight / 2.f - labelBounds.height / 2.f - labelBounds.top);
        shortcutX -= s_mouseHintIconTextGap;

        const sf::FloatRect iconLocalBounds = mouseHint.icon.getLocalBounds();
        const float shortcutIconScale = s_shortcutIconSize /
            std::max(iconLocalBounds.width, iconLocalBounds.height);
        mouseHint.icon.setScale(shortcutIconScale, shortcutIconScale);
        shortcutX -= s_shortcutIconSize;
        mouseHint.icon.setPosition(
            shortcutX + s_shortcutIconSize / 2.f,
            rowY + s_mouseHintRowHeight / 2.f);
        shortcutX -= s_groupGap;
    }
}

void HelpControlRow::layoutKeyboardShortcuts(const float rowY)
{
    float shortcutX = m_position.x + m_size.x - s_shortcutsRightPadding;

    for (int partIndex = static_cast<int>(m_shortcutParts.size()) - 1;
        partIndex >= 0; --partIndex) {
        const ShortcutPart &part = m_shortcutParts[partIndex];
        if (part.type == ShortcutPartType::Keycap) {
            Keycap &keycap = m_keycaps[part.index];
            const sf::FloatRect keycapBounds = keycap.getBounds();
            shortcutX -= keycapBounds.width;
            keycap.setPosition({shortcutX, rowY});
            shortcutX -= s_shortcutGap;

            if (part.index > 0 && std::find(m_groupEndKeycapIndices.begin(),
                m_groupEndKeycapIndices.end(), part.index) != m_groupEndKeycapIndices.end()) {
                shortcutX -= s_groupGap;
            }
        } else {
            sf::Text &separator = m_shortcutSeparators[part.index];
            const sf::FloatRect separatorBounds = separator.getLocalBounds();
            shortcutX -= separatorBounds.width;
            separator.setPosition(
                shortcutX - separatorBounds.left,
                rowY + 4.f - separatorBounds.top);
            shortcutX -= s_shortcutGap;
        }
    }
}

void HelpControlRow::layoutLegend(const float startY)
{
    float legendX = m_position.x + s_textLeftPadding;
    for (std::size_t legendIndex = 0; legendIndex < m_legendLabels.size(); ++legendIndex) {
        m_legendDots[legendIndex].setPosition(
            legendX + s_legendDotRadius,
            startY + s_legendDotRadius + 2.f);
        legendX += s_legendDotRadius * 2.f + 4.f;

        const sf::FloatRect labelBounds = m_legendLabels[legendIndex].getLocalBounds();
        m_legendLabels[legendIndex].setPosition(
            legendX - labelBounds.left,
            startY - labelBounds.top);
        legendX += labelBounds.width + 14.f;
    }
}

void HelpControlRow::refreshLayout()
{
    const sf::FloatRect iconLocal = m_iconSprite.getLocalBounds();
    const float iconScale = s_iconSize / std::max(iconLocal.width, iconLocal.height);
    m_iconSprite.setScale(iconScale, iconScale);
    m_iconSprite.setPosition(
        m_position.x + s_iconSize / 2.f,
        m_position.y + 18.f);

    m_title.setPosition(m_position.x + s_textLeftPadding, m_position.y + 8.f);
    m_subtitle.setPosition(m_position.x + s_textLeftPadding, m_position.y + 28.f);

    const bool hasMouseHints = !m_mouseHints.empty();
    const float mouseRowY = m_position.y + 8.f;
    const float keyboardRowY = hasMouseHints
        ? mouseRowY + s_mouseHintRowHeight + s_shortcutRowGap
        : m_position.y + 16.f;

    if (hasMouseHints)
        layoutMouseHints(mouseRowY);
    layoutKeyboardShortcuts(keyboardRowY);

    if (!m_legendLabels.empty()) {
        const float legendStartY = m_position.y + s_baseRowHeight
            + (hasMouseHints ? s_mouseHintRowHeight + s_shortcutRowGap : 0.f) + 2.f;
        layoutLegend(legendStartY);
    }
}

void HelpControlRow::onBase()
{
}

void HelpControlRow::onHover()
{
}

void HelpControlRow::onFocus()
{
}

void HelpControlRow::onPress()
{
}
