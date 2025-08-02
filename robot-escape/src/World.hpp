#pragma once

#include "Geometry.hpp"
#include "Canvas.hpp"

#include <ranges>
#include <vector>

template<Block tElementBlock, Block tTrailBlock = tElementBlock>
struct ElementWithPos {
    Position pos{};
    std::vector<Position> trail;
    std::string name;

    void moveTo(Position newPos) noexcept {
        trail.push_back(pos);
        if (trail.size() > 3) { trail.erase(trail.begin()); }
        pos = newPos;
    }

    void render(Canvas &canvas) const noexcept {
        canvas.setBlock(tElementBlock, pos);
        for (auto trailPos : trail) {
            auto currentBlock = canvas.blockAt(trailPos);
            if (currentBlock == Block::Room || currentBlock == Block::RobotTrail) { canvas.setBlock(tTrailBlock, trailPos); }
        }
    }
};

using Player = ElementWithPos<Block::Player, Block::PlayerTrail>;
using Robot = ElementWithPos<Block::Robot, Block::RobotTrail>;
using Exit = ElementWithPos<Block::Exit>;

struct Room {
    Rectangle rect;
};

struct Field {
    std::vector<Room> rooms;
    Rectangle rect;

    void updatePosAndSize() {
        rect = rooms.front().rect;
        for (std::size_t i = 1; i < rooms.size(); ++i) {
            rect |= rooms[i].rect;
        }
    }
    void addRoom(Rectangle roomRect) {
        rooms.emplace_back(Room{roomRect});
        updatePosAndSize();
    }
    void render(Canvas &canvas) const noexcept {
        for (const auto &room : rooms) { canvas.fillRect(room.rect.padded(1, 1), Block::Wall); }
        for (const auto &room : rooms) { canvas.fillRect(room.rect, Block::Room); }
    }
    [[nodiscard]] auto contains(Position pos) const noexcept -> bool {
        return std::ranges::any_of(rooms, [&](const auto &room) -> bool { return room.rect.contains(pos); });
    }
    template<typename Fn> auto filterPositions(Fn fn) const -> std::vector<Position> {
        std::vector<Position> result;
        result.reserve(rect.size.area());
        rect.forEach([&](const Position& pos) {
            if (contains(pos) && fn(pos)) { result.push_back(pos); }
        });
        return result;
    }
};

struct World {
    Field field;
    Player player;
    std::vector<Robot> robots;
    std::vector<Exit> exits;

    World() = default;
    template<typename T>
    [[nodiscard]] static auto tooNear(Position pos, int distance, const std::vector<T> &elements) {
        return std::any_of(elements.begin(), elements.end(), [&](const auto &element) {
            return element.pos.distanceTo(pos) <= distance;
        });
    }
    [[nodiscard]] auto isValidPlayerMovement(Position pos) const noexcept -> bool {
        return field.contains(pos);
    }
    [[nodiscard]] auto isValidRobotMovement(Position pos) const noexcept -> bool {
        return field.contains(pos) && !tooNear(pos, 0, robots);
    }
    [[nodiscard]] auto isPlayerOnExit() const noexcept -> bool {
        return std::ranges::any_of(exits, [&](const Exit &exit) -> bool { return exit.pos == player.pos; });
    }
    [[nodiscard]] auto isRobotOnPlayer() const noexcept -> bool {
        return std::ranges::any_of(robots, [&](const Robot &robot) -> bool { return robot.pos == player.pos; });
    }
    template<typename Fn>
    auto randomValidFieldPosition(Fn fn) const -> Position {
        auto validPositions = field.filterPositions(fn);
        if (validPositions.empty()) {
            throw std::logic_error{"Could not find a valid position"};
        }
        return validPositions[randomInt(0, validPositions.size() - 1)];
    }
    void addExitAtRandomPosition() {
        auto exit = Exit{randomValidFieldPosition([](Position pos){return true;})};
        exits.emplace_back(std::move(exit));
    }
    void setPlayerToRandomPosition() {
        player.pos = randomValidFieldPosition([&](Position pos) {
            return !tooNear(pos, 3, exits);
        });
    }
    void addRobotAtRandomPosition() {
        auto robot = Robot{randomValidFieldPosition([&](Position pos) {
            if (player.pos.distanceTo(pos) <= 4) { return false; }
            return !tooNear(pos, 4, exits) && !tooNear(pos, 1, robots);
        })};
        robot.name = std::format("Robot {}", robots.size() + 1);
        robots.emplace_back(std::move(robot));
    }
    void render(Canvas &canvas) const noexcept {
        canvas.setTopLeft(canvas.size.center() - field.rect.center());
        field.render(canvas);
        for (const auto &exit : exits) {
            exit.render(canvas);
        }
        player.render(canvas);
        for (const auto &robot : robots) {
            robot.render(canvas);
        }
    }
};

