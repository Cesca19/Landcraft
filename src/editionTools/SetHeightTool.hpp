//
// Created by fran on 22/06/2026.
//

#ifndef LANDCRAFT_SETHEIGHTTOOL_HPP
#define LANDCRAFT_SETHEIGHTTOOL_HPP

#include "IEditionTool.hpp"
#include "../utils/MathUtils.hpp"
#include "../commands/SetTilesCornersHeightCommand.hpp"

class SetHeightTool : public IEditionTool {
public:
    SetHeightTool(sf::Vector2f startMenuPosition);
    ~SetHeightTool() override;
    bool isEditing() const override;
    bool isSelectionLocked() const override;
    SelectionMode getRequiredSelectionMode() const override;
    void onToolSelected() const override;
    void onToolUnSelected() const override;
    bool areEditableTilesVisible() const override;
    void handleEvents(const sf::RenderWindow& window, const sf::Event &event, WorldModel &model, WorldView &view, BrushController &brushController, CommandHistory &history) override;
    void handleContinuousEvents(const sf::RenderWindow& window, WorldModel &model, WorldView &view, BrushController &brushController, CommandHistory &history) override;
    void setVisibility(bool isVisible) const override;

    void initOnSetHeightValidateCallback(std::function<void(const std::string&)> callback);
    std::string getSetHeightInputValue() const;
private:
    void startContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const BrushController &brushController);
    void updateContinuousElevation(const sf::RenderWindow& window, WorldModel &model, const WorldView &view, const BrushController &brushController);
    void stopContinuousElevation(WorldModel &model, WorldView &view, CommandHistory &history);
    void applySetHeightOnCurrentSelection(WorldModel &model, const WorldView &view, const BrushController &brushController);
    void applySetHeightAlongPath(sf::Vector2i targetPosition, WorldModel &model, const WorldView &view, const BrushController &brushController);
    void updateHeightValue(std::string newValue);

    void setSelectionMode(int index);
    void setUIVisibility(bool isVisible) const;
    void initMenuUI(sf::Vector2f startMenuPosition);
    void initSelectionModeUI(sf::Vector2f startMenuPosition);
    void initSetHeightUI(sf::Vector2f startMenuPosition);
    void initWidgetsList();

    float m_heightValue;
    bool m_isEditing;
    bool m_isSelectionLocked;
    int m_currentSelectionMode;
    float m_mouseMovementThreshold;
    sf::Clock m_continuousElevationClock;
    sf::Vector2i m_lastMouseScreenPosition;
    sf::Vector2i m_lastMouseWorldPosition;
    const sf::Mouse::Button m_editingMouseButton = sf::Mouse::Left;
    std::unique_ptr<SetTilesCornersHeightCommand> m_ongoingSetTilesCornersHeightCommand;
    std::vector<SelectionMode> m_selectionModes;
    std::vector<SpriteButton*> m_selectionModesButtons;
    std::vector<IWidget*> m_widgets;
    Box *m_setHeightToolBox;
    Text *m_setHeightToolText;
    Box *m_selectionModeBox;
    Text *m_selectionModeText;
    Text *m_setHeightText;
    TextInput *m_setHeightInput;
};


#endif //LANDCRAFT_SETHEIGHTTOOL_HPP
