//
// Created by fran on 13/03/2026.
//

#include "SelectionView.hpp"

SelectionView::SelectionView()
    : m_tileCornerRadius(5)
    , m_highlightedTilesVertexArray(sf::Triangles)
    , m_highlightedTileCorner(m_tileCornerRadius)
    // , m_highlightedTileCornerColor(255, 0, 255, 100)
    , m_highlightedTileCornerColor(255, 150, 70, 255)
    , m_highlightedTileColor(255, 150, 70, 40) // half-transparent magenta
{
    m_highlightedTileCorner.setFillColor(m_highlightedTileCornerColor);
    m_highlightedTileCorner.setOrigin(m_tileCornerRadius, m_tileCornerRadius); 
}

SelectionView::~SelectionView()
{
}

void SelectionView::drawTiles(sf::RenderWindow &window, const std::vector<Tile*> &tilesToHighlight, const Camera &camera)
{
    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(tilesToHighlight.size() * 6); // 2 triangles per tile, 3 corners per triangle

    int index = 0;

    for (const Tile* tile : tilesToHighlight) {
        // up-right triangle
        for (const TileCorner* corner : tile->getUpRightTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = m_highlightedTileColor;
        }
        
        // down-left triangle
        for (const TileCorner* corner : tile->getDownLeftTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = m_highlightedTileColor;
        }
    }
    window.draw(m_highlightedTilesVertexArray);
}

void SelectionView::drawTileCorners(sf::RenderWindow &window, const std::vector<TileCorner *> &cornersToHighlight, const Camera &camera)
{
    const float pinSize = 7.5;
    for (const TileCorner* corner : cornersToHighlight)  {
        sf::Vector2f baseScreenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        sf::Vector2f topScreenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight() + pinSize);

        const sf::Vertex line[] = {
            sf::Vertex(baseScreenPos, m_highlightedTileCornerColor),
            sf::Vertex(topScreenPos, m_highlightedTileCornerColor)
        };
        window.draw(line, 2, sf::Lines);

        m_highlightedTileCorner.setPosition(topScreenPos);
        window.draw(m_highlightedTileCorner);
    }
}