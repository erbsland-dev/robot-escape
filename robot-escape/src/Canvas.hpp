#pragma once

#include "Geometry.hpp"

#include <vector>
#include <iostream>
#include <numeric>
#include <ranges>
#include <cstdint>

enum class Block : uint8_t {
    Empty,
    Wall,
    Room,
    Exit,
    Player,
    PlayerTrail,
    Robot,
    RobotTrail,
};

struct Canvas {
    Size size;
    std::vector<Block> data;
    Position topLeft;

    constexpr static auto cDefaultSize = Size{40, 20};

    explicit Canvas(Size size = cDefaultSize) noexcept : size{size}, data(size.area(), Block::Empty) {}
    void setTopLeft(Position pos) noexcept { topLeft = pos; }
    void setBlock(Block block, Position pos) noexcept {
        if (!size.contains(pos + topLeft)) { return; }
        data[size.index(pos + topLeft)] = block;
    }
    template<typename... PosArgs>
    void setBlocks(Block block, PosArgs... pos) noexcept {
        (setBlock(block, pos), ...);
    }
    void fillRect(Rectangle rect, Block block) noexcept {
        rect.forEach([&](Position p) { setBlock(block, p); });
    }
    [[nodiscard]] auto blockAt(Position pos) const noexcept -> Block {
        return blockFromOrigin(pos + topLeft);
    }
    [[nodiscard]] auto blockFromOrigin(Position pos) const noexcept -> Block {
        if (!size.contains(pos)) { return Block::Empty; }
        return data[size.index(pos)];
    }
    void renderToConsole() const noexcept {
        std::cout << "\x1b[0m";
        auto lastBlock = Block::Player;
        for (int y = 0; y < size.height; ++y) {
            for (int x = 0; x < size.width; ++x) {
                renderBlockAt({x, y}, lastBlock);
            }
            std::cout << '\n';
        }
        std::cout << "\x1b[0m\n";
    }
    void renderBlockAt(Position pos, Block &lastBlock) const noexcept {
        static std::array<std::string_view, 16> walls = {
            "■", "╺", "╻", "┏", "╸", "━", "┓", "┳", "╹", "┗", "┃", "┣", "┛", "┻", "┫", "╋"
        };
        auto block = blockFromOrigin(pos);
        if (block != lastBlock) {
            switch (block) {
            case Block::Empty: std::cout << "\x1b[90m"; break;
            case Block::Wall: std::cout << "\x1b[32m"; break;
            case Block::Room: std::cout << "\x1b[0m"; break;;
            case Block::Exit: std::cout << "\x1b[92m"; break;
            case Block::Player: case Block::PlayerTrail: std::cout << "\x1b[93m"; break;
            case Block::Robot: case Block::RobotTrail: std::cout << "\x1b[91m"; break;
            }
        }
        switch (block) {
        case Block::Empty: std::cout << "░"; break;
        case Block::Wall: std::cout << walls[matchBlocks4(pos, Block::Wall)]; break;
        case Block::Room: std::cout << " "; break;
        case Block::Exit: std::cout << "⚑"; break;
        case Block::Player: std::cout << "☻"; break;
        case Block::Robot: std::cout << "♟"; break;
        case Block::PlayerTrail: case Block::RobotTrail: std::cout << "∙"; break;
        }
        lastBlock = block;
    }
    [[nodiscard]] auto matchBlocks4(Position pos, Block block) const noexcept -> uint32_t {
        return std::accumulate(cPosDelta4.rbegin(), cPosDelta4.rend(), 0U, [&](uint32_t acc, Position delta) {
            return (acc << 1) | (blockFromOrigin(pos + delta) == block ? 1 : 0);
        });
    }
};