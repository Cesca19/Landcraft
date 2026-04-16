//
// Created by fran on 13/03/2026.
//

#include "BrushView.hpp"

BrushView::BrushView()
    : m_tileCornerRadius(5)
    , m_highlightedTilesVertexArray(sf::Triangles)
    , m_highlightedTileCorner(m_tileCornerRadius)
    , m_highlightedTileCornerColor(255, 150, 70, 255)
    , m_highlightedTileColor(255, 150, 70, 200)
{
    m_highlightedTileCorner.setFillColor(m_highlightedTileCornerColor);
    m_highlightedTileCorner.setOrigin(m_tileCornerRadius, m_tileCornerRadius); 
}

BrushView::~BrushView()
{
}

void BrushView::drawTiles(sf::RenderWindow &window, const std::vector<Tile*> &tilesToHighlight, const Camera &camera, sf::Vector2f brushCenter, float brushRadius, const sf::Image& brushImage)
{
    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(tilesToHighlight.size() * 6); // 2 triangles per tile, 3 corners per triangle

    int index = 0;

    // Get brush parameters
    float texWidth = static_cast<float>(brushImage.getSize().x);
    float texHeight = static_cast<float>(brushImage.getSize().y);
    float minX = brushCenter.x - brushRadius;
    float minY = brushCenter.y - brushRadius;
    float boundingBoxWidth = 2.0f * brushRadius;
    if (boundingBoxWidth <= 0.0f) boundingBoxWidth = 1.0f;

    // Helper lambda to calculate the alpha for a single corner based on the brush image
    auto getVertexColor = [&](const TileCorner* corner) {
        float pixelIntensity = 0.0f;

        // If brush size is 0, highlight the exact tile uniformly
        if (brushRadius <= 0.0f) {
            pixelIntensity = 1.0f;
        } else {
            sf::Vector2f worldPos(corner->getColumn(), corner->getRow());
            float u = (worldPos.x - minX) / boundingBoxWidth;
            float v = (worldPos.y - minY) / boundingBoxWidth;
            
            // Check if the corner is inside the brush bounding box
            if (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f) {
                float pixelX = std::clamp(u * texWidth, 0.0f, texWidth - 1.0f);
                float pixelY = std::clamp(v * texHeight, 0.0f, texHeight - 1.0f);
                sf::Color pixel = brushImage.getPixel(static_cast<unsigned int>(pixelX), static_cast<unsigned int>(pixelY));
                
                // Works for both transparent PNGs and black-background PNGs
                pixelIntensity = (pixel.r / 255.0f) * (pixel.a / 255.0f);
            }
        }
        
        // Scale the intensity by 180 (out of 255) so the grid remains visible underneath
        sf::Uint8 finalAlpha = static_cast<sf::Uint8>(pixelIntensity * m_highlightedTileColor.a); 
        return sf::Color(m_highlightedTileColor.r, m_highlightedTileColor.g, m_highlightedTileColor.b, finalAlpha); 
    };

    // Apply the calculated color to each vertex
    for (const Tile* tile : tilesToHighlight) {
        for (const TileCorner* corner : tile->getUpRightTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index].color = getVertexColor(corner);
            index++;
        }
        for (const TileCorner* corner : tile->getDownLeftTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index].color = getVertexColor(corner);
            index++;
        }
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha; 
    window.draw(m_highlightedTilesVertexArray, states);
}

void BrushView::drawTileCorners(sf::RenderWindow &window, const std::vector<TileCorner *> &cornersToHighlight, const Camera &camera)
{
    const float pinSize = 2.5f;
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