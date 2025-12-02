/**
 * \file WorldScene.hpp
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/BatchRenderer2D.hpp"
#include "CS200/Camera.hpp"
#include "CS200/ImmediateRenderer2D.hpp"
#include "Engine/GameState.hpp"
#include <memory>
#include <vector>

class WorldScene : public CS230::GameState
{
public:
    void Load() override;
    void Update() override;
    void Unload() override;
    void Draw() const override;
    void DrawImGui() override;

    gsl::czstring GetName() const override
    {
        return "World Scene (HW7)";
    }

private:
    struct Player
    {
        Math::vec2  position{ 0.0, 0.0 };
        double      rotation{ 0.0 };
        CS200::RGBA color{ CS200::WHITE };
    };

    struct WorldObject
    {
        Math::vec2  position;
        Math::vec2  scale;
        double      rotation;
        CS200::RGBA color;
        bool        isCircle;
    };

    void UpdatePlayerInput(int playerIndex, double dt);

    void DrawWorld(const Math::TransformationMatrix& viewProj) const;
    void DrawViewportSeparators(Math::ivec2 windowSize) const;
    void DrawGrid(CS200::ImmediateRenderer2D& renderer) const;
    void DrawPlayerShape(CS200::ImmediateRenderer2D& renderer, const Player& p) const;

private:
    std::vector<Player>        m_players;
    std::vector<CS200::Camera> m_cameras;
    int                        m_activeViewportCount = 1;

    std::vector<WorldObject> m_worldObjects;

    CS200::BatchRenderer2D     m_batchRenderer;
    CS200::ImmediateRenderer2D m_immediateRenderer;
};