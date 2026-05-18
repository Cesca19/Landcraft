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

class TileMap  : public sf::Drawable, public sf::Transformable
{
public:
    TileMap(const std::string &tilesetFilepath, sf::Vector2u tilesSize);
    void clear();
    void init(const std::vector<std::vector<Tile>>& tiles, const Camera& camera);
    void initBrushes(const std::vector<std::string> &brushesImagePaths);
    void updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const Camera& camera);
    void updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<TileCorner *> &modifiedCorners, const Camera& camera);
    void paintTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<Tile *> & tilesToPaint, int textureId);
    void paintTile(const std::vector<std::vector<Tile>>& worldTiles, Tile *tileToPaint, int textureId);
    void setIsWireframeVisible(bool enabled);
    void setAreShadedTilesVisible(bool enabled);

    void initSplatmap(const std::string& filepath, const sf::Vector2i& tileSize, int nbCols, int nbRows);
    void drawStrokeOnSplatmap(const PaintStroke& stroke, const sf::Vector2i& tileSize, int nbCols, int nbRows);
    sf::Image getSplatmapArea(const sf::IntRect& area) const;
    void restoreSplatmapArea(const sf::IntRect& area, const sf::Image& pixels);
    const sf::Image& getSplatmapImage() const;
    void updateSplatmapImage();
private:
    void addShadedTile(const Tile& tile, const Camera& camera);
    void addWireframeTile(const Tile& tile, const Camera& camera);
    void updateTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::set<std::pair<int, int>>& tilesToUpdate, const Camera& camera);
    void updateShadedTile(const Tile& tile, const Camera& camera, int shadedIndex);
    void updateWireframeTile(const Tile& tile, const Camera& camera, int wireframeIndex);
    void paintTile(int shadedIndex, int textureId);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    
    sf::VertexArray m_shadedTilesVertexArray;
    sf::VertexArray m_wireframeTilesVertexArray;

    sf::Color m_shadedTileColor;
    sf::Color m_wireframeTileColor;

    sf::Vector2u m_texturesTilesSize;
    const sf::Texture* m_tilesetTexture;

    bool m_isWireframeVisible;
    bool m_areShadedTilesVisible;

    sf::RenderTexture m_splatmap;
    sf::Image m_splatmapImage;
    sf::Shader m_terrainShader;
    std::unordered_map<int, sf::Sprite> m_brushSprites;
    std::vector<sf::Texture> m_tilesTextures;

    sf::Vector2i m_tilesSize;
    int m_nbCols;
    int m_nbRows;
};


#endif //LANDCRAFT_TILEMAP_HPP