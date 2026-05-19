//
// Created by fran on 11/03/2026.
//

#include "TileMap.hpp"

TileMap::TileMap(const std::string &tilesetFilepath, const sf::Vector2u tilesSize)
    : m_texturesTilesSize(tilesSize)
    , m_shadedTilesVertexArray(sf::Triangles)
    , m_wireframeTilesVertexArray(sf::Lines)
    , m_shadedTileColor(sf::Color(252, 252, 254))
    , m_wireframeTileColor(sf::Color(110, 110, 120, 255))
    , m_isWireframeVisible(true)
    , m_areShadedTilesVisible(true)
{
    m_tilesetTexture = &ResourceManager::getInstance().getTexture(tilesetFilepath);
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/grass_32.png"));
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/sand_32.png"));
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/rock_32.png"));
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/snow_32.png"));

    for (auto& texture : m_tilesTextures)
        texture.setRepeated(true);
}

void TileMap::clear()
{
    m_shadedTilesVertexArray.clear();
    m_wireframeTilesVertexArray.clear();
}

void TileMap::init(const std::vector<std::vector<Tile>> &tiles, const Camera &camera)
{
    if (tiles.empty() || tiles[0].empty())
        return; // TO DO: Handle empty tile map case

    m_shadedTilesVertexArray.clear();
    m_wireframeTilesVertexArray.clear();
    for (int row = 0; row < tiles.size(); ++row)
        for (int col = 0; col < tiles[0].size(); ++col)
        {
            const Tile &tile = tiles[row][col];
            addShadedTile(tile, camera);
            addWireframeTile(tile, camera);
        }
    if (!m_terrainShader.loadFromFile("assets/shaders/terrain.vert", "assets/shaders/terrain.frag"))
        throw std::runtime_error("Failed to load terrain shader");
        m_terrainShader.setUniform("u_Splatmap", m_splatmap.getTexture());
        m_terrainShader.setUniform("u_TexGrass", m_tilesTextures[0]);
        m_terrainShader.setUniform("u_TexSand",  m_tilesTextures[1]);
        m_terrainShader.setUniform("u_TexRock",  m_tilesTextures[2]);
        m_terrainShader.setUniform("u_TexSnow",  m_tilesTextures[3]);

}

void TileMap::initBrushes(const std::vector<std::string> &brushesImagePaths)
{
    for (int i = 0; i < brushesImagePaths.size(); i++) {
        sf::Sprite sprite;
        sf::Texture& texture = ResourceManager::getInstance().getTexture(brushesImagePaths[i]);
        sprite.setTexture(texture);
        sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
        m_brushSprites.insert({i, sprite});  
    }
}

void TileMap::updatePositions(const std::vector<std::vector<Tile>> &worldTiles, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty()) return;
    int nbRows = static_cast<int>(worldTiles.size());
    int nbCols = static_cast<int>(worldTiles[0].size());

    // Detection of the map orientation to determine the correct drawing order
    sf::Vector2f p00 = camera.world_to_screen(0, 0, 0);
    sf::Vector2f p10 = camera.world_to_screen(1, 0, 0);
    sf::Vector2f p01 = camera.world_to_screen(0, 1, 0);
    bool colForward = (p10.y - p00.y) > 0;
    bool rowForward = (p01.y - p00.y) > 0;
    int rStart = rowForward ? 0 : nbRows - 1;
    int rEnd = rowForward ? nbRows : -1;
    int rStep = rowForward ? 1 : -1;
    int cStart = colForward ? 0 : nbCols - 1;
    int cEnd = colForward ? nbCols : -1;
    int cStep = colForward ? 1 : -1;

    int drawOrder = 0;
    for (int row = rStart; row != rEnd; row += rStep) {
        for (int col = cStart; col != cEnd; col += cStep) {
            const Tile &tile = worldTiles[row][col];
            int shadedIndex = drawOrder * 6; // 6 vertices per tile
            int wireframeIndex = drawOrder * 8; // 8 vertices per tile (4 lines)
            updateShadedTile(tile, camera, shadedIndex);
            updateWireframeTile(tile, camera, wireframeIndex);
            drawOrder++;
        }
    }
}

void TileMap::updatePositions(const std::vector<std::vector<Tile>>& worldTiles, const std::vector<TileCorner *> &modifiedCorners, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty() || modifiedCorners.empty()) return;
    const int nbRows = static_cast<int>(worldTiles.size());
    const int nbCols = static_cast<int>(worldTiles[0].size());
    // Get the coordinates (Row, Col) of all modified tiles
    std::set<std::pair<int, int>> tilesToUpdate;

    for (const TileCorner* corner : modifiedCorners) {
        int r = static_cast<int>(corner->getRow());
        int c = static_cast<int>(corner->getColumn());
        // A corner (r, c) can belong with up to 4 tiles :
        if (r < nbRows && c < nbCols) tilesToUpdate.insert({r, c});
        if (r < nbRows && c > 0)      tilesToUpdate.insert({r, c - 1});
        if (r > 0 && c < nbCols)      tilesToUpdate.insert({r - 1, c});
        if (r > 0 && c > 0)           tilesToUpdate.insert({r - 1, c - 1});
    }
    // only update those tiles positions
    updateTiles(worldTiles, tilesToUpdate, camera);
}

void TileMap::paintTiles(const std::vector<std::vector<Tile>> &worldTiles, const std::vector<Tile *> &tilesToPaint,
    const int textureId)
{
    if (worldTiles.empty() || worldTiles[0].empty() || tilesToPaint.empty()) return;
    const int nbCols = static_cast<int>(worldTiles[0].size());
    for (const Tile *tile : tilesToPaint) {
        sf::Vector2f tilePosition = tile->getPosition();
        if (tilePosition == sf::Vector2f{-1, -1})
            continue;
        const int tileIndex = static_cast<int>(tilePosition.y * nbCols + tilePosition.x);
        paintTile(tileIndex * 6, textureId);
    }
}

void TileMap::paintTile(const std::vector<std::vector<Tile>> &worldTiles, Tile *tileToPaint, int textureId)
{
    if (worldTiles.empty() || worldTiles[0].empty() || tileToPaint == nullptr) return;
    const int nbCols = static_cast<int>(worldTiles[0].size());
    sf::Vector2f tilePosition = tileToPaint->getPosition();
    if (tilePosition == sf::Vector2f{-1, -1})
        return;
    const int tileIndex = static_cast<int>(tilePosition.y * nbCols + tilePosition.x);
    paintTile(tileIndex * 6, textureId);
}

void TileMap::setIsWireframeVisible(bool enabled)
{
    m_isWireframeVisible = enabled;
}

void TileMap::setAreShadedTilesVisible(bool enabled)
{
    m_areShadedTilesVisible = enabled;
}

void TileMap::initSplatmap(const std::string &filepath, const sf::Vector2i &tileSize, int nbCols, int nbRows)
{
    m_tilesSize = tileSize;
    m_nbCols = nbCols;
    m_nbRows = nbRows;
    unsigned int expectedWidth = tileSize.x * nbCols;
    unsigned int expectedHeight = tileSize.y * nbRows;

    m_splatmap.create(expectedWidth, expectedHeight);
    m_splatmap.clear(sf::Color(0, 0, 0, 0));

    sf::Image loadedImage;
    if (!filepath.empty() && loadedImage.loadFromFile(filepath)) {
        if (loadedImage.getSize().x == expectedWidth && loadedImage.getSize().y == expectedHeight) {
            sf::Texture texture;
            texture.loadFromImage(loadedImage);
            sf::Sprite sprite(texture);
            sf::RenderStates states;
            states.blendMode = sf::BlendNone;
            m_splatmap.draw(sprite, states);
            std::cout << "Splatmap loaded successfully: " << filepath << std::endl;
        }
    }

    m_splatmap.display();
    m_terrainShader.setUniform("u_Splatmap", m_splatmap.getTexture());
    m_terrainShader.setUniform("u_MapSize", sf::Vector2f(static_cast<float>(nbCols), static_cast<float>(nbRows)));
    updateSplatmapImage();
}

void TileMap::drawStrokeOnSplatmap(const PaintStroke& stroke, const sf::Vector2i& tileSize, int nbCols, int nbRows)
{
    sf::Sprite& brushSprite = m_brushSprites[stroke.brushTextureId];
    sf::RenderStates states = sf::RenderStates::Default;
    float diameterInTiles = (stroke.radius * 2.0f) + 1.0f;
    float expectedPixelWidth = diameterInTiles * tileSize.x;
    float expectedPixelHeight = diameterInTiles * tileSize.y;
    
    brushSprite.setScale(
        expectedPixelWidth / brushSprite.getTexture()->getSize().x,
        expectedPixelHeight / brushSprite.getTexture()->getSize().y
    );
    // In blending mode the source color is the color of the brush sprite, 
    // and the destination color is the current color in the splatmap.
    // where new_pixel = (dest * dest_factor) [equation] (source * source_factor)
    if (stroke.textureId == 0) {
        brushSprite.setColor(sf::Color(255, 255, 255, 255)); // Eraser
        states.blendMode = sf::BlendMode(
            // reverse_subtract does : dest - source, which means it will subtract the brush color from the splatmap color, effectively erasing it
            // sf::BlendMode::One means we use 100% of the splatmap color
            // sf::BlendMode::SrcAlpha means we use the brush color multiplied by its alpha
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::ReverseSubtract, // for RGB channels : grass, sand, rock
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::ReverseSubtract // for Alpha channel : snow
        );
    } else {
        states.blendMode = sf::BlendMode(
            // Here we use the brush color multiplied by its alpha, so only the non-transparent parts of the brush will affect the splatmap
            // then we add it to the existing splatmap color,
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add,
            // here we let the spla
            sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add
        );
        // brushSprite.setColor(sf::Color(255, 0, 0, 255)) : multiplies the initial color of the brush texture by those RGB and alpha 
        // whithout overriding them so a pixel a initial 0.7 alpha will remain 0.7 but will be colored in red 
        if (stroke.textureId == 1) 
            brushSprite.setColor(sf::Color(255, 0, 0, 255)); // Grass R 
        else if (stroke.textureId == 2) 
            brushSprite.setColor(sf::Color(0, 255, 0, 255)); // Sand G
        else if (stroke.textureId == 3) 
            brushSprite.setColor(sf::Color(0, 0, 255, 255)); // Rock B
        else if (stroke.textureId == 4) 
        {
            brushSprite.setColor(sf::Color(0, 0, 0, 255));   // Snow A
            states.blendMode = sf::BlendMode(
                // we do the opposite for snow because it's stored in the alpha channel of the splatmap,
                // we want to add it to the splatmap but not affect the RGB channels, so we use Zero for RGB and SrcAlpha for Alpha
                sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add,
                sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add
            );
        }
    }
    float percentX = stroke.worldPosition.x / static_cast<float>(nbCols);
    float percentY = stroke.worldPosition.y / static_cast<float>(nbRows);
    brushSprite.setPosition(percentX * m_splatmap.getSize().x, percentY * m_splatmap.getSize().y);
    m_splatmap.draw(brushSprite, states);
    m_splatmap.display();
}

sf::Image TileMap::getSplatmapArea(const sf::IntRect& area) const 
{
    sf::Image fullImage = m_splatmap.getTexture().copyToImage();
    sf::Image subImage;
    subImage.create(area.width, area.height);
    subImage.copy(fullImage, 0, 0, area);
    return subImage;
}

void TileMap::restoreSplatmapArea(const sf::IntRect& area, const sf::Image& pixels) 
{
    sf::Texture tempTex;
    tempTex.loadFromImage(pixels);
    sf::Sprite tempSprite(tempTex);
    tempSprite.setPosition(area.left, area.top);
    
    sf::RenderStates states;
    states.blendMode = sf::BlendNone;
    m_splatmap.draw(tempSprite, states);
    m_splatmap.display();
}

const sf::Image &TileMap::getSplatmapImage() const
{
    return m_splatmapImage;
}

void TileMap::updateSplatmapImage()
{
    m_splatmapImage = m_splatmap.getTexture().copyToImage();
}

void TileMap::addShadedTile(const Tile &tile, const Camera &camera)
{
    // -> shaded tiles
    // up right triangle
    const std::vector<TileCorner *> upRightCorners = tile.getUpRightTriangleCorners();
    for (const TileCorner *corner : upRightCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        sf::Vertex vertex(screenPos, m_shadedTileColor);
        vertex.texCoords = sf::Vector2f(corner->getColumn(), corner->getRow());
        m_shadedTilesVertexArray.append(vertex);
    }

    // down left triangle
    const std::vector<TileCorner *> downLeftCorners = tile.getDownLeftTriangleCorners();
    for (const TileCorner *corner : downLeftCorners)
    {
        sf::Vector2f screenPos = camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        sf::Vertex vertex(screenPos, m_shadedTileColor);
        vertex.texCoords = sf::Vector2f(corner->getColumn(), corner->getRow());
        m_shadedTilesVertexArray.append(vertex);
    }
}

void TileMap::addWireframeTile(const Tile &tile, const Camera &camera)
{
    // -> wireframe tiles
    const std::vector<TileCorner *> corners = tile.getCorners();
    for (size_t i = 0; i < corners.size(); ++i)
    {
        const TileCorner *corner1 = corners[i];
        const TileCorner *corner2 = corners[(i + 1) % corners.size()];
        sf::Vector2f screenPos1 = camera.world_to_screen(corner1->getColumn(), corner1->getRow(), corner1->getHeight());
        sf::Vector2f screenPos2 = camera.world_to_screen(corner2->getColumn(), corner2->getRow(), corner2->getHeight());
        m_wireframeTilesVertexArray.append(sf::Vertex(screenPos1, m_wireframeTileColor));
        m_wireframeTilesVertexArray.append(sf::Vertex(screenPos2, m_wireframeTileColor));
    }
}

void TileMap::updateTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::set<std::pair<int, int>>& tilesToUpdate, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty()) 
        return;
    int nbRows = static_cast<int>(worldTiles.size());
    int nbCols = static_cast<int>(worldTiles[0].size());
    sf::Vector2f p00 = camera.world_to_screen(0, 0, 0);
    sf::Vector2f p10 = camera.world_to_screen(1, 0, 0);
    sf::Vector2f p01 = camera.world_to_screen(0, 1, 0);
    bool colForward = (p10.y - p00.y) > 0;
    bool rowForward = (p01.y - p00.y) > 0;

    for (const auto&[row, col] : tilesToUpdate) {
        const Tile& tile = worldTiles[row][col];
        int rIndex = rowForward ? row : (nbRows - 1 - row);
        int cIndex = colForward ? col : (nbCols - 1 - col);
        int drawOrder = (rIndex * nbCols) + cIndex;
        int shadedIndex = drawOrder * 6;
        int wireframeIndex = drawOrder * 8;

        // --- UPDATE TRIANGLES ---
        updateShadedTile(tile, camera, shadedIndex);
        // --- UPDATE WIREFRAME ---
        updateWireframeTile(tile, camera, wireframeIndex);
    }
}

void TileMap::updateShadedTile(const Tile &tile, const Camera &camera, int shadedIndex)
{
    for (const TileCorner* corner : tile.getUpRightTriangleCorners()) {
        m_shadedTilesVertexArray[shadedIndex].position =
            camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_shadedTilesVertexArray[shadedIndex].texCoords = sf::Vector2f(corner->getColumn(), corner->getRow());
        shadedIndex++;
    }
    for (const TileCorner* corner : tile.getDownLeftTriangleCorners()) {
        m_shadedTilesVertexArray[shadedIndex].position =
            camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_shadedTilesVertexArray[shadedIndex].texCoords = sf::Vector2f(corner->getColumn(), corner->getRow());
        shadedIndex++;
    }
}

void TileMap::updateWireframeTile(const Tile &tile, const Camera &camera, int wireframeIndex)
{
    const std::vector<TileCorner*> corners = tile.getCorners();
    for (size_t i = 0; i < corners.size(); i++) {
        const TileCorner* corner1 = corners[i];
        const TileCorner* corner2 = corners[(i + 1) % corners.size()];

        m_wireframeTilesVertexArray[wireframeIndex++].position =
            camera.world_to_screen(corner1->getColumn(), corner1->getRow(), corner1->getHeight());
        m_wireframeTilesVertexArray[wireframeIndex++].position =
            camera.world_to_screen(corner2->getColumn(), corner2->getRow(), corner2->getHeight());
    }
}

void TileMap::paintTile(int shadedIndex, const int textureId)
{
    if (textureId <= 0) {
        for (int i = 0; i < 6; i++) {
            m_shadedTilesVertexArray[shadedIndex].texCoords = sf::Vector2f(0.f, 0.f);
            m_shadedTilesVertexArray[shadedIndex].color = m_shadedTileColor;
            shadedIndex++;
        }
        return;
    }
    const unsigned int x = textureId * m_texturesTilesSize.x;
    const int y = 0;
    float offset = 0.2f; // to avoid texture bleeding
    sf::Vector2f texCoords[6] = {
        sf::Vector2f(x + offset, y + offset),
        sf::Vector2f(x + m_texturesTilesSize.x - offset, y + offset),
        sf::Vector2f(x + m_texturesTilesSize.x - offset, y + m_texturesTilesSize.y - offset),
        sf::Vector2f(x + m_texturesTilesSize.x - offset, y + m_texturesTilesSize.y - offset),
        sf::Vector2f(x + offset, y + m_texturesTilesSize.y - offset),
        sf::Vector2f(x + offset, y + offset)
    };
    for (const auto texCoord : texCoords) {
        m_shadedTilesVertexArray[shadedIndex].texCoords = texCoord;
        m_shadedTilesVertexArray[shadedIndex].color = sf::Color::White;
        shadedIndex++;
    }
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    // states.texture = m_tilesetTexture;

    // draw the vertex array
    if (m_areShadedTilesVisible) {
        states.shader = &m_terrainShader;
        target.draw(m_shadedTilesVertexArray, states);
    }

    states.shader = nullptr;
    if (m_isWireframeVisible)
        target.draw(m_wireframeTilesVertexArray, states);
}
