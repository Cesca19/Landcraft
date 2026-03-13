//
// Created by fran on 13/03/2026.
//

#include "SelectionView.hpp"

SelectionView::SelectionView()
    : m_tileCornerRadius(2.5f)
    , m_highlightedTilesVertexArray(sf::Triangles)
    , m_highlightedTileCorner(m_tileCornerRadius) // radius of 5 pixels for corner highlights
    , m_highlightedTileCornerColor(sf::Color::Magenta)
    , m_highlightedTileColor(255, 0, 255, 128) // half-transparent magenta
{
    m_highlightedTileCorner.setFillColor(m_highlightedTileCornerColor);
    m_highlightedTileCorner.setOrigin(m_tileCornerRadius, m_tileCornerRadius); 
}

SelectionView::~SelectionView()
{
}

void SelectionView::drawTiles(sf::RenderWindow &window, const std::vector<Tile> &tilesToHighlight, const Camera &camera)
{
    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(tilesToHighlight.size() * 6); // 2 triangles per tile, 3 corners per triangle

    int index = 0;

    for (const Tile& tile : tilesToHighlight) {
        // up-right triangle
        for (const TileCorner* corner : tile.getUpRightTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = m_highlightedTileColor;
        }
        
        // down-left triangle
        for (const TileCorner* corner : tile.getDownLeftTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = m_highlightedTileColor;
        }
    }
    window.draw(m_highlightedTilesVertexArray);
}

void SelectionView::drawTileCorners(sf::RenderWindow &window, const std::vector<TileCorner *> &cornersToHighlight, const Camera &camera)
{
    for (const TileCorner* corner : cornersToHighlight)  {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_highlightedTileCorner.setPosition(screenPos);
        window.draw(m_highlightedTileCorner);
    }
}
