//
// Created by fran on 11/03/2026.
//

#ifndef LANDCRAFT_TILEMAP_HPP
#define LANDCRAFT_TILEMAP_HPP

#include <set>
#include <SFML/Graphics.hpp>
#include "Camera.hpp"
#include "../../model/Tile.hpp"
#include "../../model/PaintStroke.hpp"
#include "../../utils/ResourceManager.hpp"
#include "../../utils/BrushUtils.hpp"

class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    TileMap(float minElevation, float maxElevation, float waterHeight);
    void clear();
    void init(const std::vector<std::vector<Tile>>& tiles, const Camera& camera);
    void initBrushes(const std::vector<std::string> &brushesImagePaths);
    void updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const Camera& camera);
    void updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<TileCorner *> &modifiedCorners, const Camera& camera);
    void setIsWireframeVisible(bool enabled);
    void setAreShadedTilesVisible(bool enabled);

    void initSplatMap(const std::string& filepath, const sf::Vector2i& tileSize, int nbCols, int nbRows);
    void drawStrokeOnSplatMap(const PaintStroke& stroke, const sf::Vector2i& tileSize, int nbCols, int nbRows);
    sf::Image getSplatMapArea(const sf::IntRect& area) const;
    void restoreSplatMapArea(const sf::IntRect& area, const sf::Image& pixels);
    const sf::Image& getSplatMapImage() const;
    void updateSplatMapImage();

private:
    void updateTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::set<std::pair<int, int>>& tilesToUpdate, const Camera& camera);
    void updateShadedTile(const Tile& tile, const Camera& camera, int shadedIndex);
    void updateWireframeTile(const Tile& tile, const Camera& camera, int wireframeIndex);
    void updateGround(const std::vector<std::vector<Tile>> &worldTiles, const Camera &camera);
    void updateGroundWall(const TileCorner* c1, const TileCorner* c2, const Camera& camera, int &groundIndex);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
    sf::VertexArray m_shadedTilesVertexArray;
    sf::VertexArray m_wireframeTilesVertexArray;
    sf::VertexArray m_groundVertexArray;

    sf::Color m_shadedTileColor;
    sf::Color m_wireframeTileColor;

    bool m_isWireframeVisible;
    bool m_areShadedTilesVisible;

    sf::RenderTexture m_splatmap;
    sf::Image m_splatmapImage;
    sf::Shader m_terrainShader;
    std::unordered_map<int, sf::Texture> m_processedBrushTextures;
    std::unordered_map<int, sf::Sprite> m_brushSprites;
    std::vector<sf::Texture> m_tilesTextures;

    sf::Vector2i m_tilesSize;
    int m_nbCols;
    int m_nbRows;
    float m_minElevation;
    float m_maxElevation;
    float m_waterHeight;
};


#endif //LANDCRAFT_TILEMAP_HPP