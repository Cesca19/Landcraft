//
// Created by fran on 23/06/2026.
//

#ifndef LANDCRAFT_SMOOTHHEIGHTTOOL_HPP
#define LANDCRAFT_SMOOTHHEIGHTTOOL_HPP

#include<bits/stdc++.h>
#include "IEditionTool.hpp"
#include "../utils/MathUtils.hpp"
#include "../commands/SetTilesCornersHeightCommand.hpp"

class SmoothHeightTool : public IEditionTool {
public:
    SmoothHeightTool(sf::Vector2f startMenuPosition);
    ~SmoothHeightTool() override;
    bool isEditing() const override;
    bool isSelectionLocked() const override;
    SelectionMode getRequiredSelectionMode() const override;
    void onToolSelected() const override;
    void onToolUnSelected() const override;
    bool areEditableTilesVisible() const override;
    void handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, BrushController &brushController, CommandHistory &history) override;
    void handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController, CommandHistory &history) override;
    void setVisibility(bool isVisible) const override;
private:
    void startContinuousSmoothing(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const BrushController &brushController);
    void updateContinuousSmoothing(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const BrushController &brushController);
    void stopContinuousSmoothing(WorldModel &model, WorldView &view, CommandHistory &history);
    void applySmoothingOnCurrentSelection(WorldModel &model, const WorldView &view, const BrushController &brushController);
    void applySmoothingAlongPath(sf::Vector2i targetPosition, WorldModel &model, const WorldView &view, const BrushController &brushController);
    std::unordered_map<TileCorner*, float> computeSmoothHeights(const std::vector<BrushTileCornerHit>& selection,  WorldModel &model) const;

    std::string getSmoothStrengthValue() const;
    void incrementSmoothStrength();
    void decrementSmoothStrength();
    void updateSmoothStrength(float newValue);

    void setSelectionMode(int index);
    void setUIVisibility(bool isVisible) const;
    void initMenuUI(sf::Vector2f startMenuPosition);
    void initSelectionModeUI(sf::Vector2f startMenuPosition);
    void initSmoothStrengthUI(sf::Vector2f startMenuPosition);
    void initWidgetsList();

    float m_smoothStrengthStep;
    float m_minSmoothStrength;
    float m_maxSmoothStrength;
    float m_smoothStrength;
    bool m_isEditing;
    bool m_isSelectionLocked;
    int m_currentSelectionMode;
    float m_mouseMovementThreshold;
    float m_continuousElevationInterval;
    sf::Clock m_continuousElevationClock;
    sf::Vector2i m_lastMouseScreenPosition;
    sf::Vector2i m_lastMouseWorldPosition;
    const sf::Mouse::Button m_editingMouseButton = sf::Mouse::Left;
    std::unique_ptr<SetTilesCornersHeightCommand> m_ongoingSetTilesCornersHeightCommand;
    std::vector<SelectionMode> m_selectionModes;
    std::vector<SpriteButton*> m_selectionModesButtons;
    std::vector<IWidget*> m_widgets;
    Box *m_smoothHeightToolBox;
    Text *m_smoothHeightToolText;
    Box *m_selectionModeBox;
    Text *m_selectionModeText;
    Box *m_smoothStrengthBox;
    SpriteButton *m_smoothStrengthDecreaseButton;
    SpriteButton *m_smoothStrengthIncreaseButton;
    Text *m_smoothStrengthText;
    Text *m_smoothValueText;
};


#endif //LANDCRAFT_SMOOTHHEIGHTTOOL_HPP
