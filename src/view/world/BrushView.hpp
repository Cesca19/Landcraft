//
// Created by fran on 13/03/2026.
//

#ifndef LANDCRAFT_BRUSHVIEW_HPP
#define LANDCRAFT_BRUSHVIEW_HPP

#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include <map>
#include "../../model/Tile.hpp"
#include "../../model/BrushInfos.hpp"

class BrushView {
public:
    BrushView();
    ~BrushView();
    void setOutlineThickness(float thickness);
    float getOutlineThickness() const;
    void drawTiles(sf::RenderWindow &window, const std::vector<BrushTileHit> &tilesToHighlight, const Camera &camera);
    void drawTileCorners(sf::RenderWindow &window, const std::vector<BrushTileCornerHit> &cornersToHighlight, const Camera &camera);
    void drawBrushOverlay(sf::RenderWindow &window, const std::vector<Tile*> &tilesToHighlight, const Camera &camera,
         sf::Vector2f brushCenter, float brushRadius, const sf::Image& brushImage);
    void drawMaskedSelectedTiles(sf::RenderWindow &window, const std::vector<BrushTileHit> &selectedTiles, const Camera &camera,
        sf::Vector2f brushCenter, float brushRadius, const sf::Image& brushImage);
    
private:
    void drawBrushFillPass(sf::RenderWindow &window, const sf::Texture &brushTexture);
    void drawBrushOutlinePass(sf::RenderWindow &window, const sf::Texture &brushTexture);
    const sf::Texture& getBrushTexture(const sf::Image& brushImage);
    const sf::Texture& getBrushOutlineTexture(const sf::Image& brushImage);
    
    std::map<const sf::Image*, sf::Texture> m_brushTexturesCache;
    std::map<const sf::Image*, sf::Texture> m_brushOutlineTexturesCache;

    sf::Color m_brushBaseColor;
    sf::Color m_brushOutlineColor;
    sf::Color m_brushFillLightColor;
    sf::Color m_brushFillStrongColor;
    float m_tileCornerRadius;
    sf::VertexArray m_highlightedTilesVertexArray;
    sf::CircleShape m_highlightedTileCorner;
    sf::Color m_highlightedTileCornerColor;
    sf::Color m_highlightedTileColor;

    sf::Shader m_alphaCutoffShader;
    bool m_hasShader;
    float m_outlineThickness;
    float m_brushTextureUpscale;
};

#endif //LANDCRAFT_BRUSHVIEW_HPP