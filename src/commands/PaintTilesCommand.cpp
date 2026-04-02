//
// Created by fran on 17/03/2026.
//

#include "PaintTilesCommand.hpp"

PaintTilesCommand::PaintTilesCommand(const std::vector<Tile *> &tiles, const int textureId)
    : m_textureId(textureId)
{
    for (Tile * tile: tiles) {
        if (tile->getTextureId() == textureId)
            continue;
        m_tiles.push_back(tile);
        m_previousTextureIds.push_back(tile->getTextureId());
        tile->setTextureId(textureId);
    }
}

void PaintTilesCommand::AddTiles(const std::vector<Tile *> tiles, WorldModel &model, WorldView &view)
{
    for (Tile * tile: tiles)
        AddTile(tile, model, view);
}

void PaintTilesCommand::AddTile(Tile *tile, WorldModel &model, WorldView &view)
{
    if (tile->getTextureId() == m_textureId)
        return; // skip if the tile already has the target texture to avoid unnecessary painting
    m_tiles.push_back(tile);
    m_previousTextureIds.push_back(tile->getTextureId());
    tile->setTextureId(m_textureId);
    view.paintTile(model.getTiles(), tile, m_textureId);
}

void PaintTilesCommand::execute(WorldModel &model, WorldView &view)
{
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
    return " Paint " + std::to_string(m_tiles.size()) + " tile(s) with texture " + std::to_string(m_textureId);
}

bool PaintTilesCommand::isEmpty() const
{
    return m_tiles.empty();
}