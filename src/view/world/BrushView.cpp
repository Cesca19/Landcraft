//
// Created by fran on 13/03/2026.
//

#include "BrushView.hpp"

BrushView::BrushView()
    : m_brushOutlineColor(160, 120, 220)
    , m_brushFillLightColor(160, 120, 220, 40)
    , m_brushFillStrongColor(160, 120, 220, 90)
    , m_tileCornerRadius(5)
    , m_highlightedTilesVertexArray(sf::Triangles)
    , m_highlightedTileCorner(m_tileCornerRadius)
    , m_highlightedTileCornerColor(m_brushOutlineColor)
    , m_highlightedTileColor(m_brushFillStrongColor)
{
    m_highlightedTileCorner.setFillColor(m_highlightedTileCornerColor);
    m_highlightedTileCorner.setOrigin(m_tileCornerRadius, m_tileCornerRadius); 
}

BrushView::~BrushView()
{
}

void BrushView::drawTiles(sf::RenderWindow &window, const std::vector<BrushTileHit> &tilesToHighlight, const Camera &camera)
{
    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(tilesToHighlight.size() * 6); 

    int index = 0;

    for (const BrushTileHit& hit : tilesToHighlight) {
        if (hit.weight <= 0.01f) continue;

        sf::Color currentColor = m_highlightedTileColor;
        currentColor.a = static_cast<sf::Uint8>(m_highlightedTileColor.a * hit.weight);
        const Tile* tile = hit.tile;

        // up-right triangle
        for (const TileCorner* corner : tile->getUpRightTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = currentColor;
        }

        // down-left triangle
        for (const TileCorner* corner : tile->getDownLeftTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = currentColor;
        }
    }
    window.draw(m_highlightedTilesVertexArray);
}

void BrushView::drawTileCorners(sf::RenderWindow &window, const std::vector<BrushTileCornerHit> &cornersToHighlight, const Camera &camera)
{
    const float pinSize = 2.5f;
    for (const BrushTileCornerHit& hit : cornersToHighlight)  {
        
        if (hit.weight <= 0.01f) continue;

        sf::Color currentColor = m_highlightedTileCornerColor;
        currentColor.a = static_cast<sf::Uint8>(m_highlightedTileCornerColor.a * hit.weight);

        const TileCorner* corner = hit.corner;

        sf::Vector2f baseScreenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        sf::Vector2f topScreenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight() + pinSize);

        const sf::Vertex line[] = {
            sf::Vertex(baseScreenPos, currentColor),
            sf::Vertex(topScreenPos, currentColor)
        };
        window.draw(line, 2, sf::Lines);

        m_highlightedTileCorner.setFillColor(currentColor);
        m_highlightedTileCorner.setPosition(topScreenPos);
        window.draw(m_highlightedTileCorner);
    }
}

void BrushView::drawBrushOverlay(sf::RenderWindow &window, const std::vector<Tile*> &tilesToHighlight, const Camera &camera, 
    sf::Vector2f brushCenter, float brushRadius, const sf::Image& brushImage, float overlayAlpha)
{
    if (tilesToHighlight.empty()) return;

    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(tilesToHighlight.size() * 6);

    int index = 0;
    const sf::Texture& brushTexture = getBrushTexture(brushImage);
    sf::Vector2u texSize = brushTexture.getSize();
    
    float boundingBoxWidth = (2.0f * brushRadius) + 1.0f; 
    float minX = brushCenter.x - (boundingBoxWidth / 2.0f);
    float minY = brushCenter.y - (boundingBoxWidth / 2.0f);

    // sf::Color overlayColor = m_highlightedTileColor;
    // overlayColor.a = overlayAlpha;
    
    for (const Tile* tile : tilesToHighlight) {
        auto processCorner = [&](const TileCorner* corner) {
            sf::Vector2f worldPos(corner->getColumn(), corner->getRow());
            sf::Vector2f screenPos = camera.world_to_screen(worldPos.x, worldPos.y, corner->getHeight());
            
            m_highlightedTilesVertexArray[index].position = screenPos;
            m_highlightedTilesVertexArray[index].color = m_brushFillLightColor;
            
            float u = (worldPos.x - minX) / boundingBoxWidth;
            float v = (worldPos.y - minY) / boundingBoxWidth;
            
            float texX = (u * brushImage.getSize().x) + 1.0f;
            float texY = (v * brushImage.getSize().y) + 1.0f;
            
            m_highlightedTilesVertexArray[index].texCoords = sf::Vector2f(texX, texY);
            index++;
        };

        for (const TileCorner* corner : tile->getUpRightTriangleCorners()) processCorner(corner);
        for (const TileCorner* corner : tile->getDownLeftTriangleCorners()) processCorner(corner);
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha; 
    states.texture = &brushTexture;
    window.draw(m_highlightedTilesVertexArray, states);
}

const sf::Texture& BrushView::getBrushTexture(const sf::Image& brushImage)
{
    auto it = m_brushTexturesCache.find(&brushImage);
    if (it != m_brushTexturesCache.end()) {
        return it->second;
    }

    sf::Image renderImg;
    renderImg.create(brushImage.getSize().x + 2, brushImage.getSize().y + 2, sf::Color::Transparent);
    
    for (unsigned int y = 0; y < brushImage.getSize().y; ++y) {
        for (unsigned int x = 0; x < brushImage.getSize().x; ++x) {
            sf::Color p = brushImage.getPixel(x, y);
            float intensity = (p.r / 255.0f) * (p.a / 255.0f);
            
            renderImg.setPixel(x + 1, y + 1, sf::Color(255, 255, 255, static_cast<sf::Uint8>(intensity * 255.0f)));
        }
    }
    
    sf::Texture& tex = m_brushTexturesCache[&brushImage];
    tex.loadFromImage(renderImg);
    // tex.setSmooth(true);
    // tex.generateMipmap();
    
    return tex;
}

const sf::Texture& BrushView::getBrushBorderTexture(const sf::Image& brushImage)
{
    // On retourne le cache s'il existe
    auto it = m_brushBorderTexturesCache.find(&brushImage);
    if (it != m_brushBorderTexturesCache.end()) {
        return it->second;
    }

    sf::Image renderImg;
    int w = brushImage.getSize().x;
    int h = brushImage.getSize().y;
    renderImg.create(w + 2, h + 2, sf::Color(255, 255, 255, 0)); // Transparent

    auto getWeight = [&](int x, int y) -> float {
        if (x < 0 || x >= w || y < 0 || y >= h) return 0.0f;
        sf::Color p = brushImage.getPixel(x, y);
        return (p.r / 255.0f) * (p.a / 255.0f);
    };

    int borderThickness = 4; 

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float currentWeight = getWeight(x, y);
            
            if (currentWeight > 0.05f) {
                bool isBorder = false;

                for (int dy = -borderThickness; dy <= borderThickness; ++dy) {
                    for (int dx = -borderThickness; dx <= borderThickness; ++dx) {
                        
                        if (getWeight(x + dx, y + dy) <= 0.05f) {
                            isBorder = true;
                            break;
                        }
                    }
                    if (isBorder) break; 
                }
                if (isBorder) {
                    renderImg.setPixel(x + 1, y + 1, sf::Color(255, 255, 255, 255));
                }
            }
        }
    }

    sf::Texture& tex = m_brushBorderTexturesCache[&brushImage];
    tex.loadFromImage(renderImg);
    // tex.setSmooth(true);
    // tex.generateMipmap();
    
    return tex;
}

void BrushView::drawBrushBorder(sf::RenderWindow &window, const std::vector<Tile*> &tilesToHighlight, const Camera &camera, sf::Vector2f brushCenter, float brushRadius, const sf::Image& brushImage)
{
    if (tilesToHighlight.empty()) return;

    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(tilesToHighlight.size() * 6);

    int index = 0;
    // Use our new edge-detection texture instead of the full shape
    const sf::Texture& borderTexture = getBrushBorderTexture(brushImage);
    sf::Vector2u texSize = borderTexture.getSize();

    float boundingBoxWidth = (2.0f * brushRadius) + 1.0f; 
    float minX = brushCenter.x - (boundingBoxWidth / 2.0f);
    float minY = brushCenter.y - (boundingBoxWidth / 2.0f);

    sf::Color overlayColor = sf::Color::Cyan; 
    overlayColor.a = 255; // Strong opaque border

    for (const Tile* tile : tilesToHighlight) {
        auto processCorner = [&](const TileCorner* corner) {
            sf::Vector2f worldPos(corner->getColumn(), corner->getRow());
            sf::Vector2f screenPos = camera.world_to_screen(worldPos.x, worldPos.y, corner->getHeight());
            
            m_highlightedTilesVertexArray[index].position = screenPos;
            m_highlightedTilesVertexArray[index].color = overlayColor;
            
            float u = (worldPos.x - minX) / boundingBoxWidth;
            float v = (worldPos.y - minY) / boundingBoxWidth;
            
            float texX = (u * brushImage.getSize().x) + 1.0f;
            float texY = (v * brushImage.getSize().y) + 1.0f;
            
            m_highlightedTilesVertexArray[index].texCoords = sf::Vector2f(texX, texY);
            index++;
        };

        for (const TileCorner* corner : tile->getUpRightTriangleCorners()) processCorner(corner);
        for (const TileCorner* corner : tile->getDownLeftTriangleCorners()) processCorner(corner);
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha; 
    states.texture = &borderTexture;
    window.draw(m_highlightedTilesVertexArray, states);
}