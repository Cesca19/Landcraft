//
// Created by fran on 17/03/2026.
//

#ifndef LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
#define LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP
#include "ICommand.hpp"

class EditTilesCornersHeightCommand : public ICommand
{
public:
    EditTilesCornersHeightCommand(const std::vector<TileCorner *> &corners, float heightStep);
    void addHeight(float heightStep, WorldModel &model, const WorldView &view);
    void addNewCorners(const std::vector<TileCorner *> &corners, WorldModel &model, const WorldView &view);
    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;
    std::string getName() override;
private:
    std::vector<TileCorner *> m_corners;
    float m_heightStep;
};


#endif //LANDCRAFT_SETTILESCORNERSHEIGHTCOMMAND_HPP