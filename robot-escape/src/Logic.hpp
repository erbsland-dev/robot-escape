#pragma once

#include "Canvas.hpp"
#include "World.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <limits>

struct PlayerInput {
    Position movement;

    auto operator==(const PlayerInput &other) const noexcept -> bool = default;
    auto operator!=(const PlayerInput &other) const noexcept -> bool = default;
};

constexpr auto cQuitInput = PlayerInput{Position{99, 99}};

[[nodiscard]] inline auto inputFromConsole() noexcept -> PlayerInput {
    static const auto validInputs = std::map<std::string, PlayerInput>{
        {"e", {Position{1, 0}}},
        {"s", {Position{0, 1}}},
        {"w", {Position{-1, 0}}},
        {"n", {Position{0, -1}}},
        {"q", cQuitInput},
    };
    PlayerInput playerInput;
    while (playerInput == PlayerInput{}) {
        std::cout << "Enter your move (n/e/s/w/q=quit): ";
        std::cout.flush();
        std::string input;
        std::getline(std::cin, input);
        auto it = validInputs.find(input);
        if (it != validInputs.end()) {
            playerInput = it->second;
        } else {
            std::cout << "Invalid input. Please try again.\n";
        }
    }
    return playerInput;
}

enum class GameState {
    Running,
    PlayerWon,
    RobotsWon,
};

struct PlayerLogic {
    void advance(PlayerInput input, World &world) {
        auto newPlayerPos = world.player.pos + input.movement;
        if (world.isValidPlayerMovement(newPlayerPos)) {
            world.player.moveTo(newPlayerPos);
        } else {
            std::cout << "Could not move in this direction. You lost one move.\n";
        }
    }
};

struct RobotLogic {
    void advance(Robot &robot, World &world) {
        if (robot.pos == world.player.pos) { return; }
        int bestDistance = std::numeric_limits<int>::max();
        std::vector<Position> bestMoves;
        for (auto delta : cPosDelta4) {
            auto pos = robot.pos + delta;
            if (!world.isValidRobotMovement(pos)) continue;
            auto dist = pos.distanceTo(world.player.pos);
            if (dist < bestDistance) { bestMoves = {pos}; bestDistance = dist; }
            else if (dist == bestDistance) { bestMoves.push_back(pos); }
        }
        if (bestMoves.empty()) return;
        robot.moveTo(bestMoves[randomInt(0, bestMoves.size() - 1)]);
    }
};

struct Logic {
    World world;
    PlayerLogic playerLogic;
    RobotLogic robotLogic;

    explicit Logic(World &&initialWorld) : world{std::move(initialWorld)} {}

    void advance(PlayerInput input) {
        playerLogic.advance(input, world);
        for (auto &robot : world.robots) {
            robotLogic.advance(robot, world);
        }
    }

    [[nodiscard]] auto gameState() const noexcept -> GameState {
        if (world.isPlayerOnExit()) { return GameState::PlayerWon; }
        if (world.isRobotOnPlayer()) { return GameState::RobotsWon; }
        return GameState::Running;
    }

    void render(Canvas &canvas) const {
        world.render(canvas);
    }
};