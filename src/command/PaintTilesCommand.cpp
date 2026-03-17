//
// Created by fran on 17/03/2026.
//

#include "PaintTilesCommand.hpp"

PaintTilesCommand::PaintTilesCommand(const std::vector<Tile *> tiles, const int textureId)
    : m_tiles(tiles)
    , m_textureId(textureId)
{
    for (Tile * tile: m_tiles)
        m_previousTextureIds.push_back(tile->getTextureId());
}

void PaintTilesCommand::execute(WorldModel &model, WorldView &view)
{
    for (Tile * tile: m_tiles)
        tile->setTextureId(m_textureId);
    view.paintTiles(model.getTiles(), m_tiles, m_textureId);
}

void PaintTilesCommand::undo(WorldModel &model, WorldView &view)
{
    for (std::size_t i = 0; i < m_tiles.size(); ++i) { 
        m_tiles[i]->setTextureId(m_previousTextureIds[i]);
        view.paintTile(model.getTiles(), m_tiles[i], m_previousTextureIds[i]);
    }
}

std::string PaintTilesCommand::getName()
{
    return "Paint Tiles";
}
