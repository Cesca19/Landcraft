//
// Created by fran on 11/03/2026.
//

#include "TileMap.hpp"

TileMap::TileMap(const float minElevation, const float maxElevation, const float waterHeight)
    : m_shadedTilesVertexArray(sf::Triangles)
    , m_wireframeTilesVertexArray(sf::Lines)
    , m_groundVertexArray(sf::Triangles)
    , m_shadedTileColor(sf::Color(252, 252, 254))
    , m_wireframeTileColor(sf::Color(110, 110, 120, 255))
    , m_isWireframeVisible(true)
    , m_areShadedTilesVisible(true)
    , m_nbCols(0)
    , m_nbRows(0)
    , m_minElevation(minElevation)
    , m_maxElevation(maxElevation)
    , m_waterHeight(waterHeight)
{
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/grass_32.png"));
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/sand_32.png"));
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/rock_32.png"));
    m_tilesTextures.push_back(ResourceManager::getInstance().getTexture("assets/textures/snow_32.png"));

    for (auto &texture: m_tilesTextures)
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

    const int nbTiles = static_cast<int>(tiles.size() * (tiles[0].size()));
    // shaded
    m_shadedTilesVertexArray.resize(nbTiles * 6);
    // wireframe
    m_wireframeTilesVertexArray.resize(nbTiles * 8);
    // ground
    const int nbWall = static_cast<int>((tiles.size())* 2 + (tiles[0].size() * 2));
    m_groundVertexArray.resize(nbWall * 6);

    updatePositions(tiles, camera);
    updateGround(tiles, camera);

    if (!m_terrainShader.loadFromFile("assets/shaders/terrain.vert", "assets/shaders/terrain.frag"))
        throw std::runtime_error("Failed to load terrain shader");
    m_terrainShader.setUniform("u_SplatMap", m_splatmap.getTexture());
    m_terrainShader.setUniform("u_TexGrass", m_tilesTextures[0]);
    m_terrainShader.setUniform("u_TexSand",  m_tilesTextures[1]);
    m_terrainShader.setUniform("u_TexRock",  m_tilesTextures[2]);
    m_terrainShader.setUniform("u_TexSnow",  m_tilesTextures[3]);
    m_terrainShader.setUniform("u_MinElevation", m_minElevation);
    m_terrainShader.setUniform("u_ElevationRange", m_maxElevation - m_minElevation);
    m_terrainShader.setUniform("u_WaterHeight", m_waterHeight);
    m_terrainShader.setUniform("u_WireframeColor", 
        sf::Glsl::Vec4(m_wireframeTileColor.r / 255.0f, m_wireframeTileColor.g / 255.0f, 
        m_wireframeTileColor.b / 255.0f, m_wireframeTileColor.a / 255.0f));
    m_terrainShader.setUniform("u_ShowGrid", m_isWireframeVisible ? 1.0f : 0.0f);
}

void TileMap::initBrushes(const std::vector<std::string> &brushesImagePaths)
{
    for (int i = 0; i < brushesImagePaths.size(); i++) {
        sf::Image img = ResourceManager::getInstance().getTexture(brushesImagePaths[i]).copyToImage();
        BrushUtils::sanitizeBrushImage(img);
        m_processedBrushTextures[i].loadFromImage(img);
        m_processedBrushTextures[i].setSmooth(true);

        sf::Sprite sprite;
        sprite.setTexture(m_processedBrushTextures[i]);
        sprite.setOrigin(m_processedBrushTextures[i].getSize().x / 2.0f, m_processedBrushTextures[i].getSize().y / 2.0f);
        m_brushSprites.insert({i, sprite});  
    }
}

void TileMap::updatePositions(const std::vector<std::vector<Tile>> &worldTiles, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty()) return;
    const int nbRows = static_cast<int>(worldTiles.size());
    const int nbCols = static_cast<int>(worldTiles[0].size());

    // Detection of the map orientation to determine the correct drawing order
    const sf::Vector2f p00 = camera.world_to_screen(0, 0, 0);
    const sf::Vector2f p10 = camera.world_to_screen(1, 0, 0);
    const sf::Vector2f p01 = camera.world_to_screen(0, 1, 0);
    const bool colForward = (p10.y - p00.y) > 0;
    const bool rowForward = (p01.y - p00.y) > 0;
    const int rStart = rowForward ? 0 : nbRows - 1;
    const int rEnd = rowForward ? nbRows : -1;
    const int rStep = rowForward ? 1 : -1;
    const int cStart = colForward ? 0 : nbCols - 1;
    const int cEnd = colForward ? nbCols : -1;
    const int cStep = colForward ? 1 : -1;

    int drawOrder = 0;
    for (int row = rStart; row != rEnd; row += rStep) {
        for (int col = cStart; col != cEnd; col += cStep) {
            const Tile &tile = worldTiles[row][col];
            const int shadedIndex = drawOrder * 6; // 6 vertices per tile
            const int wireframeIndex = drawOrder * 8; // 8 vertices per tile (4 lines)
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

void TileMap::setIsWireframeVisible(const bool enabled)
{
    m_isWireframeVisible = enabled;
}

void TileMap::setAreShadedTilesVisible(const bool enabled)
{
    m_areShadedTilesVisible = enabled;
}

void TileMap::initSplatMap(const std::string &filepath, const sf::Vector2i &tileSize, const int nbCols, const int nbRows)
{
    m_tilesSize = tileSize;
    m_nbCols = nbCols;
    m_nbRows = nbRows;
    const unsigned int expectedWidth = tileSize.x * nbCols;
    const unsigned int expectedHeight = tileSize.y * nbRows;

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
            std::cout << "SplatMap loaded successfully: " << filepath << std::endl;
        }
    }

    m_splatmap.display();
    m_terrainShader.setUniform("u_SplatMap", m_splatmap.getTexture());
    m_terrainShader.setUniform("u_MapSize", sf::Vector2f(static_cast<float>(nbCols), static_cast<float>(nbRows)));
    updateSplatMapImage();
}

void TileMap::drawStrokeOnSplatMap(const PaintStroke& stroke, const sf::Vector2i& tileSize, const int nbCols, const int nbRows)
{
    sf::Sprite& brushSprite = m_brushSprites[stroke.brushTextureId];
    sf::RenderStates states = sf::RenderStates::Default;

    const float diameterInTiles = (stroke.radius * 2.0f) + 1.0f;
    const float expectedPixelWidth = diameterInTiles * tileSize.x;
    const float expectedPixelHeight = diameterInTiles * tileSize.y;
    
    brushSprite.setScale(
        expectedPixelWidth / brushSprite.getTexture()->getSize().x,
        expectedPixelHeight / brushSprite.getTexture()->getSize().y
    );
    const float percentX = stroke.worldPosition.x / static_cast<float>(nbCols);
    const float percentY = stroke.worldPosition.y / static_cast<float>(nbRows);
    brushSprite.setPosition(percentX * m_splatmap.getSize().x, percentY * m_splatmap.getSize().y);

    if (stroke.textureId == 0) {
        // --- ERASER (Multiplication by the inverse of the brush) ---
        brushSprite.setColor(sf::Color(255, 255, 255, 255));
        states.blendMode = sf::BlendMode(
            sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add, // Removes RGB
            sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add  // Removes snow
        );
        m_splatmap.draw(brushSprite, states);
    } 
    else if (stroke.textureId == 4) {
        // --- SNOW ---
        brushSprite.setColor(sf::Color(255, 255, 255, 255));
        states.blendMode = sf::BlendMode(
            sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add, // Removes RGB underneath
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add           // Adds snow
        );
        m_splatmap.draw(brushSprite, states);
    } 
    else {
        // --- GRASS, SAND, ROCK ---
        // Carve out a hole (removes existing RGB and snow)
        brushSprite.setColor(sf::Color(255, 255, 255, 255));
        states.blendMode = sf::BlendMode(
            sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add,
            sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add
        );
        m_splatmap.draw(brushSprite, states);
        // Fill the hole with the pure color
        if (stroke.textureId == 1)      brushSprite.setColor(sf::Color(255, 0, 0, 255));
        else if (stroke.textureId == 2) brushSprite.setColor(sf::Color(0, 255, 0, 255));
        else if (stroke.textureId == 3) brushSprite.setColor(sf::Color(0, 0, 255, 255));
        
        states.blendMode = sf::BlendMode(
            sf::BlendMode::SrcAlpha, sf::BlendMode::One, sf::BlendMode::Add,
            sf::BlendMode::Zero, sf::BlendMode::One, sf::BlendMode::Add
        );
        m_splatmap.draw(brushSprite, states);
    }
    m_splatmap.display();
}

sf::Image TileMap::getSplatMapArea(const sf::IntRect& area) const
{
    const sf::Image fullImage = m_splatmap.getTexture().copyToImage();
    sf::Image subImage;
    subImage.create(area.width, area.height);
    subImage.copy(fullImage, 0, 0, area);
    return subImage;
}

void TileMap::restoreSplatMapArea(const sf::IntRect& area, const sf::Image& pixels)
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

const sf::Image &TileMap::getSplatMapImage() const
{
    return m_splatmapImage;
}

void TileMap::updateSplatMapImage()
{
    m_splatmapImage = m_splatmap.getTexture().copyToImage();
}

void TileMap::setWaterHeight(float height)
{
    m_waterHeight = height;
    m_terrainShader.setUniform("u_WaterHeight", m_waterHeight);
}

void TileMap::updateTiles(const std::vector<std::vector<Tile>>& worldTiles, const std::set<std::pair<int, int>>& tilesToUpdate, const Camera &camera)
{
    if (worldTiles.empty() || worldTiles[0].empty()) 
        return;
    const int nbRows = static_cast<int>(worldTiles.size());
    const int nbCols = static_cast<int>(worldTiles[0].size());
    const sf::Vector2f p00 = camera.world_to_screen(0, 0, 0);
    const sf::Vector2f p10 = camera.world_to_screen(1, 0, 0);
    const sf::Vector2f p01 = camera.world_to_screen(0, 1, 0);
    const bool colForward = (p10.y - p00.y) > 0;
    const bool rowForward = (p01.y - p00.y) > 0;

    for (const auto&[row, col] : tilesToUpdate) {
        const Tile& tile = worldTiles[row][col];
        const int rIndex = rowForward ? row : (nbRows - 1 - row);
        const int cIndex = colForward ? col : (nbCols - 1 - col);
        const int drawOrder = (rIndex * nbCols) + cIndex;
        const int shadedIndex = drawOrder * 6;
        const int wireframeIndex = drawOrder * 8;

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
        const float z = corner->getHeight();
        const float normalizedZ = (z - m_minElevation) / (m_maxElevation - m_minElevation);
        const sf::Uint8 alphaZ = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ * 255.0f)));

        m_shadedTilesVertexArray[shadedIndex].position =
            camera.world_to_screen(corner->getColumn(), corner->getRow(), corner->getHeight());
        m_shadedTilesVertexArray[shadedIndex].texCoords = sf::Vector2f(corner->getColumn(), corner->getRow());
        m_shadedTilesVertexArray[shadedIndex].color = m_shadedTileColor;
        m_shadedTilesVertexArray[shadedIndex].color.a = alphaZ;
        shadedIndex++;
    }
    for (const TileCorner* corner : tile.getDownLeftTriangleCorners()) {
        const float z = corner->getHeight();
        const float normalizedZ = (z - m_minElevation) / (m_maxElevation - m_minElevation);
        const sf::Uint8 alphaZ = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ * 255.0f)));

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
        const float z1 = corner1->getHeight();
        const float normalizedZ1 = (z1 - m_minElevation) / (m_maxElevation - m_minElevation);
        const sf::Uint8 alphaZ1 = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ1 * 255.0f)));

        // encode height for vertex2
        const float z2 = corner2->getHeight();
        const float normalizedZ2 = (z2 - m_minElevation) / (m_maxElevation - m_minElevation);
        const sf::Uint8 alphaZ2 = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, normalizedZ2 * 255.0f)));

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
    if (worldTiles.empty() || worldTiles[0].empty())
        return;
    const int nbRows = static_cast<int>(worldTiles.size());
    const int nbCols = static_cast<int>(worldTiles[0].size());
    int groundIndex = 0;

    const sf::Vector2f p00 = camera.world_to_screen(0, 0, 0);
    const sf::Vector2f p10 = camera.world_to_screen(1, 0, 0);
    const sf::Vector2f p01 = camera.world_to_screen(0, 1, 0);
    const bool colForward = (p10.y - p00.y) > 0;
    const bool rowForward = (p01.y - p00.y) > 0;

    if (colForward)
        for (int row = 0; row < nbRows; row++)
            updateGroundWall(worldTiles[row][nbCols - 1].getCorners()[1], worldTiles[row][nbCols - 1].getCorners()[2], camera, groundIndex);
    else
        for (int row = 0; row < nbRows; row++)
            updateGroundWall(worldTiles[row][0].getCorners()[0], worldTiles[row][0].getCorners()[3], camera, groundIndex);

    if (rowForward)
        for (int col = 0; col < nbCols; col++)
            updateGroundWall(worldTiles[nbRows - 1][col].getCorners()[3], worldTiles[nbRows - 1][col].getCorners()[2], camera, groundIndex);
    else
        for (int col = 0; col < nbCols; col++)
            updateGroundWall(worldTiles[0][col].getCorners()[0], worldTiles[0][col].getCorners()[1], camera, groundIndex);

    // 3. Hide unused vertices
    // Since m_groundVertexArray was resized in init() to hold 4 walls, but we only draw 2,
    // we collapse the remaining vertices to the center of the screen to make them invisible.
    while (groundIndex < m_groundVertexArray.getVertexCount()) {
        m_groundVertexArray[groundIndex++].position = sf::Vector2f(0.f, 0.f);
    }
}

void TileMap::updateGroundWall(const TileCorner *c1, const TileCorner *c2, const Camera &camera, int &groundIndex)
{
    const float groundBottomZ = m_minElevation - 5.0f;
    // const float z1 = c1->getHeight();
    // const float z2 = c2->getHeight();
    const float z1 = std::max(c1->getHeight(), m_waterHeight);
    const float z2 = std::max(c2->getHeight(), m_waterHeight);

    const sf::Vector2f top1 = camera.world_to_screen(c1->getColumn(), c1->getRow(), z1);
    const sf::Vector2f top2 = camera.world_to_screen(c2->getColumn(), c2->getRow(), z2);
    const sf::Vector2f bottom1 = camera.world_to_screen(c1->getColumn(), c1->getRow(), groundBottomZ);
    const sf::Vector2f bottom2 = camera.world_to_screen(c2->getColumn(), c2->getRow(), groundBottomZ);

    // with 255 alpha the water shader won't pass the wall
    const auto groundColor = sf::Color(203, 203, 205);

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
        nonConstShader->setUniform("u_ShowGrid", m_isWireframeVisible ? 1.0f : 0.0f);
        target.draw(m_shadedTilesVertexArray, states);
    }

    // Draw Wireframe Grid with Shader filtering
    if (m_isWireframeVisible && !m_areShadedTilesVisible) {
        states.shader = nonConstShader;
        if (nonConstShader) nonConstShader->setUniform("u_IsWireframe", 1.0f);
        nonConstShader->setUniform("u_IsWireframe", 1.0f);
        target.draw(m_wireframeTilesVertexArray, states);
    }
}