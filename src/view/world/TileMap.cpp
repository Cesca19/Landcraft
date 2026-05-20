//
// Created by fran on 11/03/2026.
//

#include "TileMap.hpp"

TileMap::TileMap(float minElevation, float maxElevation, float waterHeight)
    : m_shadedTilesVertexArray(sf::Triangles)
    , m_wireframeTilesVertexArray(sf::Lines)
    , m_groundVertexArray(sf::Triangles)
    , m_shadedTileColor(sf::Color(252, 252, 254))
    , m_wireframeTileColor(sf::Color(110, 110, 120, 255))
    , m_isWireframeVisible(true)
    , m_areShadedTilesVisible(true)
    , m_minElevation(minElevation)
    , m_maxElevation(maxElevation)
    , m_waterHeight(waterHeight)
{
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
    m_groundVertexArray.clear();
}

void TileMap::init(const std::vector<std::vector<Tile>> &tiles, const Camera &camera)

{
    if (tiles.empty() || tiles[0].empty())
        return; // TO DO: Handle empty tile map case

    m_shadedTilesVertexArray.clear();
    m_wireframeTilesVertexArray.clear();
    m_groundVertexArray.clear();

    int nbTiles = (tiles.size()) * (tiles[0].size());
    // shaded
    m_shadedTilesVertexArray.resize(nbTiles * 6);
    // wireframe
    m_wireframeTilesVertexArray.resize(nbTiles * 8);
    // ground
    int nbWall = (tiles.size())* 2 + (tiles[0].size() * 2);
    m_groundVertexArray.resize(nbWall * 6);

    updatePositions(tiles, camera);
    updateGround(tiles, camera);

    // for (int row = 0; row < tiles.size(); ++row)
    //     for (int col = 0; col < tiles[0].size(); ++col)
    //     {
    //         const Tile &tile = tiles[row][col];
    //         updateShadedTile(tile, camera, (row * tiles[0].size() + col) * 6);
    //         updateWireframeTile(tile, camera, (row * tiles[0].size() + col) * 8);
    //         // addShadedTile(tile, camera);
    //         // addWireframeTile(tile, camera);
    //     }
    
    
    if (!m_terrainShader.loadFromFile("assets/shaders/terrain.vert", "assets/shaders/terrain.frag"))
        throw std::runtime_error("Failed to load terrain shader");
    m_terrainShader.setUniform("u_Splatmap", m_splatmap.getTexture());
    m_terrainShader.setUniform("u_TexGrass", m_tilesTextures[0]);
    m_terrainShader.setUniform("u_TexSand",  m_tilesTextures[1]);
    m_terrainShader.setUniform("u_TexRock",  m_tilesTextures[2]);
    m_terrainShader.setUniform("u_TexSnow",  m_tilesTextures[3]);
    m_terrainShader.setUniform("u_MinElevation", m_minElevation);
    m_terrainShader.setUniform("u_ElevationRange", m_maxElevation - m_minElevation);
    m_terrainShader.setUniform("u_WaterHeight", m_waterHeight);
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
    updateGround(worldTiles, camera);
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

void TileMap::setIsWireframeVisible(bool enabled)
{
    m_isWireframeVisible = enabled;
}

void TileMap::setAreShadedTilesVisible(bool enabled)
{
    m_areShadedTilesVisible = enabled;
}

void TileMap::initSplatMap(const std::string &filepath, const sf::Vector2i &tileSize, int nbCols, int nbRows)
{
    m_tilesSize = tileSize;
    m_nbCols = nbCols;
    m_nbRows = nbRows;
    unsigned int expectedWidth = tileSize.x * nbCols;
    unsigned int expectedHeight = tileSize.y * nbRows;

    m_splatmap.create(expectedWidth, expectedHeight);
    m_splatmap.setSmooth(true);
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
    float percentX = stroke.worldPosition.x / static_cast<float>(nbCols);
    float percentY = stroke.worldPosition.y / static_cast<float>(nbRows);
    brushSprite.setPosition(percentX * m_splatmap.getSize().x, percentY * m_splatmap.getSize().y);

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
        m_splatmap.draw(brushSprite, states);
    } else if (stroke.textureId == 4) {
        brushSprite.setColor(sf::Color(0, 0, 0, 255)); // Snow
        // we do the opposite of rgb for snow because it's stored in the alpha channel of the splatmap,
        // we want to add it to the splatmap but not affect the RGB channels, 
        // so we use Zero for RGB and SrcAlpha for Alpha
        states.blendMode = sf::BlendMode(
            sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add,
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add
        );
        m_splatmap.draw(brushSprite, states);
    } else {
        // First we dig a hole in the splatmap existing colors
        brushSprite.setColor(sf::Color(255, 255, 255, 255)); // Subtraction mask
        states.blendMode = sf::BlendMode(
            // Here we want to subtract the brush color (multiplied by its alpha) from the splatmap color, 
            // but only for the RGB channels, so we use ReverseSubtract for RGB and Add zero to Alpha to keep it intact
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::ReverseSubtract, // Subtract existing RGB
            sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add                  // Keep Alpha (Snow) intact
        );
        m_splatmap.draw(brushSprite, states); 

        // Then we fill the hole with the new texture color
        if (stroke.textureId == 1)
            brushSprite.setColor(sf::Color(255, 0, 0, 255)); // Grass R 
        else if (stroke.textureId == 2)
            brushSprite.setColor(sf::Color(0, 255, 0, 255)); // Sand G
        else if (stroke.textureId == 3)
            brushSprite.setColor(sf::Color(0, 0, 255, 255)); // Rock B
        states.blendMode = sf::BlendMode(
            // Here we use the brush color multiplied by its alpha, so only 
            // the non-transparent parts of the brush will affect the splatmap
            // then we add it to the existing splatmap color,
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add,
            // here we let the alpha intact
            sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add
        );
        m_splatmap.draw(brushSprite, states);
    }
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
    updateGround(worldTiles, camera);
}

void TileMap::updateShadedTile(const Tile &tile, const Camera &camera, int shadedIndex)
{
    for (const TileCorner* corner : tile.getUpRightTriangleCorners()) {
        // we encode the altitude Z in the alpha channel of the vertex color for ex,
        // we remap it from [-20, 30] to [0, 255]
        float z = corner->getHeight();
        float normalizedZ = (z - m_minElevation) / (m_maxElevation - m_minElevation);
        sf::Uint8 alphaZ = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ * 255.0f)));

        m_shadedTilesVertexArray[shadedIndex].position =
            camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_shadedTilesVertexArray[shadedIndex].texCoords = sf::Vector2f(corner->getColumn(), corner->getRow());
        m_shadedTilesVertexArray[shadedIndex].color = m_shadedTileColor;
        m_shadedTilesVertexArray[shadedIndex].color.a = alphaZ;
        shadedIndex++;
    }
    for (const TileCorner* corner : tile.getDownLeftTriangleCorners()) {
        float z = corner->getHeight();
        float normalizedZ = (z - m_minElevation) / (m_maxElevation - m_minElevation);
        sf::Uint8 alphaZ = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ * 255.0f)));

        m_shadedTilesVertexArray[shadedIndex].position =
            camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_shadedTilesVertexArray[shadedIndex].texCoords = sf::Vector2f(corner->getColumn(), corner->getRow());
        m_shadedTilesVertexArray[shadedIndex].color = m_shadedTileColor;
        m_shadedTilesVertexArray[shadedIndex].color.a = alphaZ;
        shadedIndex++;
    }
}

void TileMap::updateWireframeTile(const Tile &tile, const Camera &camera, int wireframeIndex)
{
    const std::vector<TileCorner*> corners = tile.getCorners();
    for (size_t i = 0; i < corners.size(); i++) {
        const TileCorner* corner1 = corners[i];
        const TileCorner* corner2 = corners[(i + 1) % corners.size()];

        // encode height for vertex1
        float z1 = corner1->getHeight();
        float normalizedZ1 = (z1 - m_minElevation) / (m_maxElevation - m_minElevation);
        sf::Uint8 alphaZ1 = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ1 * 255.0f)));

        // encode height for vertex2
        float z2 = corner2->getHeight();
        float normalizedZ2 = (z2 - m_minElevation) / (m_maxElevation - m_minElevation);
        sf::Uint8 alphaZ2 = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ2 * 255.0f)));

        m_wireframeTilesVertexArray[wireframeIndex].position =
            camera.world_to_screen(corner1->getColumn(), corner1->getRow(), corner1->getHeight());
        m_wireframeTilesVertexArray[wireframeIndex].color = m_wireframeTileColor;
        m_wireframeTilesVertexArray[wireframeIndex].color.a = alphaZ1;
        wireframeIndex++;

        m_wireframeTilesVertexArray[wireframeIndex].position =
            camera.world_to_screen(corner2->getColumn(), corner2->getRow(), corner2->getHeight());
        m_wireframeTilesVertexArray[wireframeIndex].color = m_wireframeTileColor;
        m_wireframeTilesVertexArray[wireframeIndex].color.a = alphaZ2;
        wireframeIndex++;
    }
}

void TileMap::updateGround(const std::vector<std::vector<Tile>> &worldTiles, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty()) return;

    int nbRows = static_cast<int>(worldTiles.size());
    int nbCols = static_cast<int>(worldTiles[0].size());
    int groundIndex = 0;

    for (int col = 0; col < nbCols; ++col) {
        updateGroundWall(worldTiles[0][col].getCorners()[0], worldTiles[0][col].getCorners()[1], camera, groundIndex);
        updateGroundWall(worldTiles[nbRows - 1][col].getCorners()[3], worldTiles[nbRows - 1][col].getCorners()[2], camera, groundIndex);
    }
    for (int row = 0; row < nbRows; ++row) {
        updateGroundWall(worldTiles[row][0].getCorners()[0], worldTiles[row][0].getCorners()[3], camera, groundIndex);
        updateGroundWall(worldTiles[row][nbCols - 1].getCorners()[1], worldTiles[row][nbCols - 1].getCorners()[2], camera, groundIndex);
    }
}

void TileMap::updateGroundWall(const TileCorner *c1, const TileCorner *c2, const Camera &camera, int &groundIndex)
{
    float groundBottomZ = m_minElevation - 5.0f;
    float z1 = c1->getHeight();
    float z2 = c2->getHeight();

    sf::Vector2f top1 = camera.world_to_screen(c1->getColumn(), c1->getRow(), z1);
    sf::Vector2f top2 = camera.world_to_screen(c2->getColumn(), c2->getRow(), z2);
    sf::Vector2f bottom1 = camera.world_to_screen(c1->getColumn(), c1->getRow(), groundBottomZ);
    sf::Vector2f bottom2 = camera.world_to_screen(c2->getColumn(), c2->getRow(), groundBottomZ);

    // with 255 alpha the water shader won't pass the wall
    sf::Color groundColor(130, 130, 130, 255); 

    // Triangle 1
    m_groundVertexArray[groundIndex].position = top1;
    m_groundVertexArray[groundIndex].color = groundColor;
    m_groundVertexArray[groundIndex++].texCoords = sf::Vector2f(c1->getColumn(), c1->getRow());
    m_groundVertexArray[groundIndex].position = top2;
    m_groundVertexArray[groundIndex].color = groundColor;
    m_groundVertexArray[groundIndex++].texCoords = sf::Vector2f(c2->getColumn(), c2->getRow());
    m_groundVertexArray[groundIndex].position = bottom1;
    m_groundVertexArray[groundIndex].color = groundColor;
    m_groundVertexArray[groundIndex++].texCoords = sf::Vector2f(c1->getColumn(), c1->getRow());
    // Triangle 2
    m_groundVertexArray[groundIndex].position = top2;
    m_groundVertexArray[groundIndex].color = groundColor;
    m_groundVertexArray[groundIndex++].texCoords = sf::Vector2f(c2->getColumn(), c2->getRow());
    m_groundVertexArray[groundIndex].position = bottom2;
    m_groundVertexArray[groundIndex].color = groundColor;
    m_groundVertexArray[groundIndex++].texCoords = sf::Vector2f(c2->getColumn(), c2->getRow());
    m_groundVertexArray[groundIndex].position = bottom1;
    m_groundVertexArray[groundIndex].color = groundColor;
    m_groundVertexArray[groundIndex++].texCoords = sf::Vector2f(c1->getColumn(), c1->getRow());
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    auto* nonConstShader = const_cast<sf::Shader*>(&m_terrainShader);

    // Draw Shaded Terrain
    if (m_areShadedTilesVisible) {
        states.shader = nullptr;
        states.texture = nullptr;
        target.draw(m_groundVertexArray, states); 

        states.shader = nonConstShader;
        if (nonConstShader) nonConstShader->setUniform("u_IsWireframe", 0.0f);
        target.draw(m_shadedTilesVertexArray, states);
    }

    // Draw Wireframe Grid with Shader filtering
    if (m_isWireframeVisible) {
        states.shader = nonConstShader;
        if (nonConstShader) nonConstShader->setUniform("u_IsWireframe", 1.0f);
        target.draw(m_wireframeTilesVertexArray, states);
    }
}