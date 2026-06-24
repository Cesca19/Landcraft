//
// Created by fran on 24/06/2026.
//

#ifndef LANDCRAFT_RESETTERRAINTOOL_HPP
#define LANDCRAFT_RESETTERRAINTOOL_HPP

#include "IEditionTool.hpp"
#include "../commands/SetSplatMapCommand.hpp"
#include "../commands/SetTerrainHeightMapCommand.hpp"

class ResetTerrainTool : public IEditionTool {
public:
    ResetTerrainTool(sf::Vector2f startMenuPosition, WorldModel *model, WorldView *view, CommandHistory *history);
    ~ResetTerrainTool() override;

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
    void resetTerrainSplatMap(WorldModel &model, WorldView &view, CommandHistory &history);
    void resetTerrainHeight(WorldModel &model, WorldView &view, CommandHistory &history);
    std::vector<std::vector<float>> getDefaultHeightmap(WorldModel &model) const;
    void setUIVisibility(bool isVisible) const;
    void initMenuUI(sf::Vector2f startMenuPosition);
    void initWidgetsList();

    WorldModel *m_model;
    WorldView *m_view;
    CommandHistory *m_history;
    std::vector<IWidget*> m_widgets;
    Box *m_resetToolBox;
    Text *m_resetToolText;
    SpriteButton *m_resetSplatMapButton;
    SpriteButton *m_resetHeightButton;
};

#endif //LANDCRAFT_RESETTERRAINTOOL_HPP