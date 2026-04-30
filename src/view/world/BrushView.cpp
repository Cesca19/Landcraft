//
// Created by fran on 13/03/2026.
//

#include "BrushView.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

BrushView::BrushView()
    : m_brushOutlineColor(120, 70, 200, 220)
    , m_brushFillLightColor(160, 120, 220, 80)
    , m_brushFillStrongColor(140, 90, 210, 110)
    , m_tileCornerRadius(5)
    , m_highlightedTilesVertexArray(sf::Triangles)
    , m_highlightedTileCorner(m_tileCornerRadius)
    , m_highlightedTileCornerColor(m_brushOutlineColor)
    , m_highlightedTileColor(m_brushFillStrongColor)
    , m_hasShader(false)
    , m_outlineThickness(9.0f)
    , m_brushTextureUpscale(1.0f)
{
    m_highlightedTileCorner.setFillColor(m_highlightedTileCornerColor);
    m_highlightedTileCorner.setOrigin(m_tileCornerRadius, m_tileCornerRadius);

    if (sf::Shader::isAvailable()) {
        if (!m_alphaCutoffShader.loadFromFile("assets/shaders/brush.frag", sf::Shader::Fragment)) {
            std::cerr << "[Erreur] Echec du chargement du shader BrushView depuis assets/shaders/brush.frag, fallback sans contour shader." << std::endl;
            m_hasShader = false;
        } else {
            m_hasShader = true;
        }
    }
}

BrushView::~BrushView()
{
}

void BrushView::setOutlineThickness(const float thickness)
{
    const float clamped = std::clamp(thickness, 2.0f, 120.0f);
    if (m_outlineThickness == clamped) return;
    m_outlineThickness = clamped;
    m_brushOutlineTexturesCache.clear();
}

float BrushView::getOutlineThickness() const
{
    return m_outlineThickness;
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
        for (const TileCorner* corner : tile->getUpRightTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = currentColor;
        }

        for (const TileCorner* corner : tile->getDownLeftTriangleCorners()) {
            m_highlightedTilesVertexArray[index].position = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
            m_highlightedTilesVertexArray[index++].color = currentColor;
        }
    }
    window.draw(m_highlightedTilesVertexArray);
}

void BrushView::drawTileCorners(sf::RenderWindow &window, const std::vector<BrushTileCornerHit> &cornersToHighlight, const Camera &camera)
{
    for (const auto&[corner, weight] : cornersToHighlight)  {
        constexpr float pinSize = 3.5f;

        if (weight <= 0.01f) continue;

        sf::Color currentColor = m_highlightedTileCornerColor;
        currentColor.a = static_cast<sf::Uint8>(m_highlightedTileCornerColor.a * weight);

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
                                const sf::Vector2f brushCenter, const float brushRadius, const sf::Image& brushImage)
{
    if (tilesToHighlight.empty()) return;
    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(tilesToHighlight.size() * 6);
    int index = 0;
    const sf::Texture& brushTexture = getBrushTexture(brushImage);
    const sf::Texture& outlineTexture = getBrushOutlineTexture(brushImage);
    float boundingBoxWidth = (2.0f * brushRadius) + 1.0f;
    float minX = brushCenter.x - boundingBoxWidth / 2.0f;
    float minY = brushCenter.y - boundingBoxWidth / 2.0f;
    const float padding = 64.0f;

    sf::Color color = m_brushFillLightColor;

    for (const Tile* tile : tilesToHighlight) {
        auto processCorner = [&](const TileCorner* corner)
        {
            sf::Vector2f worldPos(corner->getColumn(), corner->getRow());
            sf::Vector2f screenPos = camera.world_to_screen(worldPos.x, worldPos.y, corner->getHeight());
            float u = (worldPos.x - minX) / boundingBoxWidth;
            float v = (worldPos.y - minY) / boundingBoxWidth;
            float texX = (u * brushImage.getSize().x * m_brushTextureUpscale) + padding;
            float texY = (v * brushImage.getSize().y * m_brushTextureUpscale) + padding;

            m_highlightedTilesVertexArray[index].position = screenPos;
            m_highlightedTilesVertexArray[index].color = color;
            m_highlightedTilesVertexArray[index].texCoords = {texX, texY};
            index++;
        };
        for (auto c : tile->getUpRightTriangleCorners()) processCorner(c);
        for (auto c : tile->getDownLeftTriangleCorners()) processCorner(c);
    }

    drawBrushFillPass(window, brushTexture);
    drawBrushOutlinePass(window, outlineTexture);
}

void BrushView::drawBrushFillPass(sf::RenderWindow &window, const sf::Texture &brushTexture)
{
    sf::RenderStates states;
    states.texture = &brushTexture;
    states.blendMode = sf::BlendAlpha;
    sf::Color fallbackColor = m_brushFillLightColor;

    fallbackColor.a = 125;
    for (std::size_t i = 0; i < m_highlightedTilesVertexArray.getVertexCount(); ++i)
        m_highlightedTilesVertexArray[i].color = fallbackColor;
    window.draw(m_highlightedTilesVertexArray, states);
}

void BrushView::drawBrushOutlinePass(sf::RenderWindow &window, const sf::Texture &brushTexture)
{
    sf::RenderStates states;
    states.texture = &brushTexture;
    states.blendMode = sf::BlendAlpha;

    // Neutral vertex color for outline pass (alpha is managed by shader).
    for (std::size_t i = 0; i < m_highlightedTilesVertexArray.getVertexCount(); ++i)
        m_highlightedTilesVertexArray[i].color = m_brushOutlineColor;
    window.draw(m_highlightedTilesVertexArray, states);
}

const sf::Texture& BrushView::getBrushTexture(const sf::Image& brushImage)
{
    const auto it = m_brushTexturesCache.find(&brushImage);
    if (it != m_brushTexturesCache.end()) {
        return it->second;
    }

    constexpr int padding = 64;
    const int w = static_cast<int>(brushImage.getSize().x);
    const int h = static_cast<int>(brushImage.getSize().y);
    const int upW = static_cast<int>(m_brushTextureUpscale) * w;
    const int upH = static_cast<int>(m_brushTextureUpscale) * h;
    const int fullW = upW + padding * 2;
    const int fullH = upH + padding * 2;

    std::vector<sf::Uint8> pixels(fullW * fullH * 4, 0);
    // Transparent background initialized to white to avoid dark halos in filtered mip levels.
    for (std::size_t i = 0; i < pixels.size(); i += 4) {
        pixels[i + 0] = 255;
        pixels[i + 1] = 255;
        pixels[i + 2] = 255;
        pixels[i + 3] = 0;
    }

    const sf::Uint8* origPixels = brushImage.getPixelsPtr();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int origIdx = (y * w + x) * 4;
            const int newIdx = ((y + padding) * fullW + (x + padding)) * 4;

            const sf::Uint8 a = origPixels[origIdx + 3];
            pixels[newIdx + 3] = a;
            if (a == 0) {
                // Keep transparent texels bright to prevent dark fringes when minified.
                pixels[newIdx + 0] = 255;
                pixels[newIdx + 1] = 255;
                pixels[newIdx + 2] = 255;
            } else {
                pixels[newIdx + 0] = origPixels[origIdx + 0];
                pixels[newIdx + 1] = origPixels[origIdx + 1];
                pixels[newIdx + 2] = origPixels[origIdx + 2];
            }
        }
    }

    sf::Image renderImg;
    renderImg.create(fullW, fullH, pixels.data());

    sf::Texture& tex = m_brushTexturesCache[&brushImage];
    tex.loadFromImage(renderImg);
    tex.setSmooth(true);

    tex.generateMipmap();
    tex.setRepeated(false);

    return tex;
}

const sf::Texture& BrushView::getBrushOutlineTexture(const sf::Image& brushImage)
{
    auto it = m_brushOutlineTexturesCache.find(&brushImage);
    if (it != m_brushOutlineTexturesCache.end()) {
        return it->second;
    }

    constexpr int padding = 64;
    const int w = static_cast<int>(brushImage.getSize().x);
    const int h = static_cast<int>(brushImage.getSize().y);
    const int fullW = w + padding * 2;
    const int fullH = h + padding * 2;
    // Keep CPU outline generation bounded to avoid frame stalls/freezes.
    const int t = std::clamp(static_cast<int>(std::round(m_outlineThickness * 1.5f)), 2, 18);

    std::vector<sf::Uint8> outlinePixels(fullW * fullH * 4, 0);
    const sf::Uint8* src = brushImage.getPixelsPtr();

    auto alphaAt = [&](int x, int y) -> sf::Uint8 {
        if (x < 0 || y < 0 || x >= w || y >= h) return 0;
        return src[(y * w + x) * 4 + 3];
    };

    const float strongOutlineAlpha = static_cast<float>(std::max<sf::Uint8>(m_brushOutlineColor.a, 220));

    static constexpr int dirX[8] = {1, -1, 0, 0, 1, -1, 1, -1};
    static constexpr int dirY[8] = {0, 0, 1, -1, 1, 1, -1, -1};

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            constexpr sf::Uint8 alphaThreshold = 16;
            const bool centerInside = alphaAt(x, y) > alphaThreshold;
            float minOutsideDist = static_cast<float>(t) + 1.0f;

            if (centerInside) {
                // Sample along principal directions only for inside pixels.
                for (int d = 0; d < 8; ++d) {
                    const int dx = dirX[d];
                    const int dy = dirY[d];
                    for (int s = 1; s <= t; ++s) {
                        if (alphaAt(x + dx * s, y + dy * s) <= alphaThreshold) {
                            minOutsideDist = std::min(minOutsideDist, static_cast<float>(s));
                            break;
                        }
                    }
                }
            }

            const int dst = ((y + padding) * fullW + (x + padding)) * 4;
            sf::Uint8 outlineA = 0;
            if (centerInside && minOutsideDist <= static_cast<float>(t)) {
                // Smooth falloff toward inner side to reduce stair-stepping when zoomed out.
                const float width = static_cast<float>(std::max(1, t));
                const float weight = std::clamp(1.0f - (minOutsideDist / (width + 0.5f)), 0.0f, 1.0f);
                outlineA = static_cast<sf::Uint8>(std::round(strongOutlineAlpha * weight));
            }
            outlinePixels[dst + 0] = 255;
            outlinePixels[dst + 1] = 255;
            outlinePixels[dst + 2] = 255;
            outlinePixels[dst + 3] = outlineA;
        }
    }

    sf::Image outImg;
    outImg.create(fullW, fullH, outlinePixels.data());

    sf::Texture& tex = m_brushOutlineTexturesCache[&brushImage];
    tex.loadFromImage(outImg);
    tex.setSmooth(true);
    tex.generateMipmap();
    tex.setRepeated(false);
    return tex;
}

void BrushView::drawMaskedSelectedTiles(sf::RenderWindow &window, const std::vector<BrushTileHit> &selectedTiles, const Camera &camera,
                                        const sf::Vector2f brushCenter, const float brushRadius, const sf::Image& brushImage)
{
    if (selectedTiles.empty()) return;

    m_highlightedTilesVertexArray.clear();
    m_highlightedTilesVertexArray.resize(selectedTiles.size() * 6);

    int index = 0;
    const sf::Texture& brushTexture = getBrushTexture(brushImage);

    float boundingBoxWidth = (2.0f * brushRadius) + 1.0f;
    float minX = brushCenter.x - (boundingBoxWidth / 2.0f);
    float minY = brushCenter.y - (boundingBoxWidth / 2.0f);

    const float padding = 64.0f;
    const float margin = 16.0f;

    for (const BrushTileHit& hit : selectedTiles) {
        if (hit.weight <= 0.01f) continue;

        const Tile* tile = hit.tile;

        sf::Color tileColor = m_brushFillStrongColor;
        tileColor.a = static_cast<sf::Uint8>(tileColor.a * hit.weight);

        auto processCorner = [&](const TileCorner* corner) {
            sf::Vector2f worldPos(corner->getColumn(), corner->getRow());
            sf::Vector2f screenPos = camera.world_to_screen(worldPos.x, worldPos.y, corner->getHeight());

            float u = (worldPos.x - minX) / boundingBoxWidth;
            float v = (worldPos.y - minY) / boundingBoxWidth;

            float texX = (u * brushImage.getSize().x * m_brushTextureUpscale) + padding;
            float texY = (v * brushImage.getSize().y * m_brushTextureUpscale) + padding;

            m_highlightedTilesVertexArray[index].position = screenPos;
            m_highlightedTilesVertexArray[index].color = tileColor;
            m_highlightedTilesVertexArray[index].texCoords = sf::Vector2f(texX, texY);
            index++;
        };

        for (const TileCorner* corner : tile->getUpRightTriangleCorners()) processCorner(corner);
        for (const TileCorner* corner : tile->getDownLeftTriangleCorners()) processCorner(corner);
    }

    sf::RenderStates states;
    states.blendMode = sf::BlendAlpha;
    states.texture = &brushTexture;

    if (m_hasShader) {
        m_alphaCutoffShader.setUniform("currentTexture", sf::Shader::CurrentTexture);
        float texW = brushTexture.getSize().x;
        float texH = brushTexture.getSize().y;
        m_alphaCutoffShader.setUniform("textureSize", sf::Vector2f(texW, texH));
        m_alphaCutoffShader.setUniform("validBounds", sf::Glsl::Vec4(
            (padding - margin) / texW,
            (padding - margin) / texH,
            (padding + brushImage.getSize().x * m_brushTextureUpscale + margin) / texW,
            (padding + brushImage.getSize().y * m_brushTextureUpscale + margin) / texH
        ));
        m_alphaCutoffShader.setUniform("outlineThickness", m_outlineThickness * m_brushTextureUpscale);
        m_alphaCutoffShader.setUniform("outlineColor", sf::Glsl::Vec4(
            m_brushOutlineColor.r / 255.f,
            m_brushOutlineColor.g / 255.f,
            m_brushOutlineColor.b / 255.f,
            m_brushOutlineColor.a / 255.f
        ));
        states.shader = &m_alphaCutoffShader;
    }
    window.draw(m_highlightedTilesVertexArray, states);
}
