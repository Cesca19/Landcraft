//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_PAINTTILESCOMMAND_HPP
#define LANDCRAFT_PAINTTILESCOMMAND_HPP

#include "ICommand.hpp"

class PaintTilesCommand : public ICommand
{
public:
    PaintTilesCommand(const std::vector<Tile *> &tiles, int textureId);
    void AddTiles(std::vector<Tile *> tiles, WorldModel &model, WorldView &view);
    void AddTile(Tile *tile, WorldModel &model, WorldView &view);
    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;
    std::string getName() override;
    bool isEmpty() const;
    Tile* getLastPaintedTile() const;
private:
    std::vector<Tile *> m_tiles;
    int m_textureId;
    std::vector<int> m_previousTextureIds;
};


#endif //LANDCRAFT_PAINTTILESCOMMAND_HPP