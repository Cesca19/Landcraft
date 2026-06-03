//
// Created by fran on 03/06/2026.
//

#ifndef LANDCRAFT_GENERATETERRAINCOMMAND_HPP
#define LANDCRAFT_GENERATETERRAINCOMMAND_HPP

#include <vector>
#include "ICommand.hpp"

class SetTerrainHeightMapCommand : public ICommand {
public:
    SetTerrainHeightMapCommand(const std::vector<std::vector<float>>& heightmap);
    void execute(WorldModel& model, WorldView& view) override;
    void undo(WorldModel& model, WorldView& view) override;
    std::string getName() override;
private:
    void applyHeightmapToModel(WorldModel& model, WorldView& view, const std::vector<std::vector<float>>& heightmap);
    std::vector<std::vector<float>> m_heightmap;
    std::vector<std::vector<float>> m_previousHeightmap;
};


#endif //LANDCRAFT_GENERATETERRAINCOMMAND_HPP