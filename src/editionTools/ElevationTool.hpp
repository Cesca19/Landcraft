//
// Created by fran on 24/03/2026.
//

#ifndef LANDCRAFT_ELEVATIONTOOL_HPP
#define LANDCRAFT_ELEVATIONTOOL_HPP

#include "IEditionTool.hpp"
#include "../commands/EditTilesCornersHeightCommand.hpp"
#include "../ui/UIFactory.hpp"

class ElevationTool : public IEditionTool {
public:
    ElevationTool(sf::Vector2f startMenuPosition);
    ~ElevationTool();
    bool isEditing() const override;
    bool isSelectionLocked() const override;
    SelectionMode getRequiredSelectionMode() const override;
    void onToolSelected() const override;
    void onToolUnSelected() const override;
    void handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, SelectionController &selectionController, CommandHistory &history) override;
    void handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, SelectionController &selectionController, CommandHistory &history) override;
private:
    void handleSelectionModeEditingEvents(const sf::Event& event);
    void handleHeightStepEditingEvents(const sf::Event& event);
    void handleHeightEditingEvents(const sf::RenderWindow& window, WorldModel& model, WorldView& view, const SelectionController &selectionController, CommandHistory &history);
    void startContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const SelectionController &selectionController, float heightStep);
    void updateContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const SelectionController &selectionController, float heightStep);
    void applyElevationOnCurrentSelection(WorldModel &model, const WorldView &view, const SelectionController &selectionController, float heightStep) const;
    void applyElevationAlongPath(const sf::Vector2i& currentWorldPosition, WorldModel &model, const WorldView &view, const SelectionController &selectionController, float heightStep) const;
    void stopContinuousElevation(WorldModel &model, WorldView &view, CommandHistory &history);
    std::set<TileCorner*> getTilesCornersFromBresenhamLine(sf::Vector2i startPosition, sf::Vector2i endPosition, WorldModel &model) const;

    void setUIVisibility(bool isVisible) const;
    void initButtonStyle(SpriteButton *button, HighlightTextAlign align = HighlightTextAlign::Top);
    void initSelectionModeUI(sf::Vector2f startMenuPosition);
    void initDigOrElevateUI(sf::Vector2f startMenuPosition, sf::Vector2f startButtonPosition);
    void initElevationStepUI(sf::Vector2f startMenuPosition, sf::Vector2f startButtonPosition);
    void initToolWidgetsList();

    std::string getHeightStepValue() const;
    void setSelectionMode(int index);
    void incrementHeightStepFactor();
    void decrementHeightStepFactor();
    void updateHeightStepFactor(int newValue);
    void dig();
    void elevate();

    bool m_shouldDig;
    bool m_shouldElevate;

    float m_heightStep;
    int m_heightStepFactor;
    int m_maxHeightStepFactor;

    bool m_isEditing;
    bool m_isSelectionLocked;
    int m_currentSelectionMode;

    float m_continuousElevationInterval;
    sf::Clock m_continuousElevationClock;

    sf::Vector2i m_lastMouseScreenPosition;
    sf::Vector2i m_lastMouseWorldPosition;

    float m_mouseMovementThreshold;
    const sf::Mouse::Button m_editingMouseButton = sf::Mouse::Left;
    std::unique_ptr<EditTilesCornersHeightCommand> m_ongoingEditCornersHeightCommand;

    std::vector<SelectionMode> m_selectionModes;
    std::vector<SpriteButton*> m_selectionModesButtons;
    SpriteButton *m_elevationStepIncrement;
    SpriteButton *m_elevationStepDecrement;
    SpriteButton *m_digButton;
    SpriteButton *m_elevateButton;
    Box *m_elevationToolBox;
    Box *m_selectionModeBox;
    Box *m_elevationStepBox;
    Box *m_digOrElevateBox;
    Text *m_elevationToolText;
    Text *m_selectionModeText;
    Text *m_elevationStepText;
    Text *m_elevationStepValueText;
    Text *m_digOrElevateText;
    std::vector<IWidget*> m_widgets;
};


#endif //LANDCRAFT_ELEVATIONTOOL_HPP