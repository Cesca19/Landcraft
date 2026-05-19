//
// Created by fran on 15/05/2026.
//

#include "PaintSplatMapCommand.hpp"

PaintSplatMapCommand::PaintSplatMapCommand()
    : m_isFirstExecution(true) 
{
}

void PaintSplatMapCommand::addStroke(const PaintStroke& stroke)
{
    m_strokes.push_back(stroke);
}

bool PaintSplatMapCommand::isEmpty() const
{
    return m_strokes.empty();
}

void PaintSplatMapCommand::drawRealTime(WorldModel& model, WorldView& view)
{
    if (m_strokes.empty())
        return;
    int nbCols = model.getTiles()[0].size();
    int nbRows = model.getTiles().size();
    sf::Vector2i tileSize = model.getTilesSize();

    view.drawStrokeOnSplatmap(m_strokes.back(), model.getTilesSize(), nbCols, nbRows);
}

void PaintSplatMapCommand::execute(WorldModel& model, WorldView& view)
{
    int nbCols = model.getTiles()[0].size();
    int nbRows = model.getTiles().size();
    sf::Vector2i tileSize = model.getTilesSize();

    if (m_isFirstExecution && !m_strokes.empty()) {
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();

        for (const auto& stroke : m_strokes) {
            float diameterInTiles = (stroke.radius * 2.0f) + 1.0f;
            float pixelRadiusX = (diameterInTiles * tileSize.x) / 2.0f;
            float pixelRadiusY = (diameterInTiles * tileSize.y) / 2.0f;
            float pX = stroke.worldPosition.x * tileSize.x;
            float pY = stroke.worldPosition.y * tileSize.y;
            if (pX - pixelRadiusX < minX) minX = pX - pixelRadiusX;
            if (pY - pixelRadiusY < minY) minY = pY - pixelRadiusY;
            if (pX + pixelRadiusX > maxX) maxX = pX + pixelRadiusX;
            if (pY + pixelRadiusY > maxY) maxY = pY + pixelRadiusY;
        }
        int rectLeft = std::max(0, static_cast<int>(minX));
        int rectTop = std::max(0, static_cast<int>(minY));
        int rectWidth = std::min((nbCols * tileSize.x) - rectLeft, static_cast<int>(maxX - minX));
        int rectHeight = std::min((nbRows * tileSize.y) - rectTop, static_cast<int>(maxY - minY));

        m_modifiedArea = sf::IntRect(rectLeft, rectTop, rectWidth, rectHeight);
        if (rectWidth > 0 && rectHeight > 0) {
            m_oldPixels.create(rectWidth, rectHeight);
            m_oldPixels.copy(view.getSplatmapImage(), 0, 0, m_modifiedArea);
        }
        m_isFirstExecution = false;
        view.updateSplatmapImage();
        return;
    }
    for (const auto& stroke : m_strokes)
        view.drawStrokeOnSplatmap(stroke, tileSize, nbCols, nbRows);
    view.updateSplatmapImage();
}

void PaintSplatMapCommand::undo(WorldModel& model, WorldView& view)
{
    if (m_modifiedArea.width > 0 && m_modifiedArea.height > 0) {
        view.restoreSplatmapArea(m_modifiedArea, m_oldPixels);
        view.updateSplatmapImage();
    }
}

std::string PaintSplatMapCommand::getName()
{
    return "Painted " + std::to_string(m_strokes.size()) + " stroke(s)";
}
