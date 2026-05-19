//
// Created by fran on 19/05/2026.
//

#include "WaterView.hpp"

WaterView::WaterView()
    : m_waterVertexArray(sf::Triangles)
    , m_waterColor(0, 120, 200, 100)
    , m_isVisible(true)
    , m_totalTime(0.0f)
{
}

WaterView::~WaterView()
{
}

void WaterView::init(int nbCols, int nbRows, const sf::Vector2i& tilesSize, float waterHeight, const Camera &camera)
{
    m_nbCols = nbCols -1;
    m_nbRows = nbRows -1;
    m_tileSize = tilesSize;
    m_waterHeight = waterHeight;

    m_waterVertexArray.clear();
    m_waterVertexArray.resize(m_nbCols * m_nbRows * 6);

    int index = 0;
    for (int row = 0; row < m_nbRows; ++row) {
        for (int col = 0; col < m_nbCols; ++col) {
            for (int k = 0; k < 6; k++)
                m_waterVertexArray[index + k].color = m_waterColor;
            index += 6;
        }
    }

    if (!m_waterShader.loadFromFile("assets/shaders/water.vert", "assets/shaders/water.frag"))
        std::cerr << "Failed to load water shaders!" << std::endl;
    else {
        sf::Texture &tex = ResourceManager::getInstance().getTexture("assets/textures/water_32.png");
        tex.setRepeated(true);
        m_waterShader.setUniform("u_WaterTexture", tex);
        m_waterShader.setUniform("u_Time", m_totalTime);
        m_waterShader.setUniform("u_DebugMode", 1);
    }
    updatePositions(camera);
}

void WaterView::updatePositions(const Camera &camera)
{
    if (!m_isVisible) 
        return;
    if (m_nbCols <= 0 || m_nbRows <= 0) return;

    sf::Vector2f p00 = camera.world_to_screen(0, 0, 0);
    sf::Vector2f p10 = camera.world_to_screen(1, 0, 0);
    sf::Vector2f p01 = camera.world_to_screen(0, 1, 0);
    bool colForward = (p10.y - p00.y) > 0;
    bool rowForward = (p01.y - p00.y) > 0;
    
    int rStart = rowForward ? 0 : m_nbRows - 1;
    int rEnd   = rowForward ? m_nbRows : -1;
    int rStep  = rowForward ? 1 : -1;
    int cStart = colForward ? 0 : m_nbCols - 1;
    int cEnd   = colForward ? m_nbCols : -1;
    int cStep  = colForward ? 1 : -1;

    int drawOrder = 0;
    for (int row = rStart; row != rEnd; row += rStep) {
        for (int col = cStart; col != cEnd; col += cStep) {
            
            int vertexIndex = drawOrder * 6;

            // Project the 4 corners of the current water cell at the fixed water altitude Z
            sf::Vector2f top    = camera.world_to_screen(col,     row,     m_waterHeight);
            sf::Vector2f right  = camera.world_to_screen(col + 1, row,     m_waterHeight);
            sf::Vector2f bottom = camera.world_to_screen(col + 1, row + 1, m_waterHeight);
            sf::Vector2f left   = camera.world_to_screen(col,     row + 1, m_waterHeight);

            // Triangle 1: Top -> Right -> Left
            m_waterVertexArray[vertexIndex + 0].position = top;
            m_waterVertexArray[vertexIndex + 1].position = right;
            m_waterVertexArray[vertexIndex + 2].position = left;

            // Triangle 2: Right -> Bottom -> Left
            m_waterVertexArray[vertexIndex + 3].position = right;
            m_waterVertexArray[vertexIndex + 4].position = bottom;
            m_waterVertexArray[vertexIndex + 5].position = left;

            // Send local grid coordinates as UVs to feed the shader wave equation
            m_waterVertexArray[vertexIndex + 0].texCoords = sf::Vector2f(col, row);
            m_waterVertexArray[vertexIndex + 1].texCoords = sf::Vector2f(col + 1, row);
            m_waterVertexArray[vertexIndex + 2].texCoords = sf::Vector2f(col, row + 1);
            m_waterVertexArray[vertexIndex + 3].texCoords = sf::Vector2f(col + 1, row);
            m_waterVertexArray[vertexIndex + 4].texCoords = sf::Vector2f(col + 1, row + 1);
            m_waterVertexArray[vertexIndex + 5].texCoords = sf::Vector2f(col, row + 1);

            drawOrder++;
        }
    }
}

void WaterView::update(float deltaTime)
{
    m_totalTime += deltaTime;
    if (!m_isVisible) 
        return;
    m_waterShader.setUniform("u_Time", m_totalTime);
}

void WaterView::draw(sf::RenderWindow &window) const
{
    if (!m_isVisible) 
        return;
    sf::RenderStates states = sf::RenderStates::Default;
    states.shader = &m_waterShader;
    window.draw(m_waterVertexArray, states);
}

void WaterView::setWaterHeight(float height) 
{ 
    m_waterHeight = height; 
}

float WaterView::getWaterHeight() const 
{ 
    return m_waterHeight; 
}

void WaterView::setIsVisible(bool visible) 
{
    m_isVisible = visible; 
}

bool WaterView::isVisible() const 
{ 
    return m_isVisible; 
}