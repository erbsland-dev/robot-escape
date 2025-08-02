#pragma once

#include <functional>
#include <array>
#include <random>
#include <thread>

inline auto randomInt(int minimum, int maximum) noexcept -> int {
    thread_local std::mt19937 rng(std::random_device{}());
    return std::uniform_int_distribution<int>{minimum, maximum}(rng);
}

struct Position {
    int x{};
    int y{};

    Position() = default;
    constexpr Position(int x, int y) noexcept : x{x}, y{y} {}
    auto operator==(const Position &other) const noexcept -> bool = default;
    auto operator!=(const Position &other) const noexcept -> bool = default;
    auto operator+(const Position &other) const noexcept -> Position { return {x + other.x, y + other.y}; }
    auto operator-(const Position &other) const noexcept -> Position { return {x - other.x, y - other.y}; }
    [[nodiscard]] auto distanceTo(Position other) const noexcept -> int {
        return std::abs(x - other.x) + std::abs(y - other.y);
    }
    [[nodiscard]] auto componentMax(Position other) const noexcept -> Position {
        return {std::max(x, other.x), std::max(y, other.y)};
    }
    [[nodiscard]] auto componentMin(Position other) const noexcept -> Position {
        return {std::min(x, other.x), std::min(y, other.y)};
    }
};

constexpr static std::array<Position, 4> cPosDelta4 = {
    {{1, 0}, {0, 1}, {-1, 0}, {0, -1}}};

struct Size {
    int width{};
    int height{};

    Size() = default;
    constexpr Size(int width, int height) noexcept : width{width}, height{height} {}
    constexpr Size(Position pos1, Position pos2) noexcept : width{pos2.x - pos1.x}, height{pos2.y - pos1.y} {}
    [[nodiscard]] auto fitsInto(Size other) const noexcept -> bool {
        return width <= other.width && height <= other.height;
    }
    [[nodiscard]] auto area() const noexcept -> int { return width * height; }
    [[nodiscard]] auto center() const noexcept -> Position {
        return {width / 2, height / 2};
    }
    [[nodiscard]] auto componentMax(Size other) const noexcept -> Size {
        return {std::max(width, other.width), std::max(height, other.height)};
    }
    [[nodiscard]] auto contains(const Position& pos) const noexcept -> bool {
        return pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height;
    }
    [[nodiscard]] auto index(const Position& pos) const noexcept -> int {
        return pos.y * width + pos.x;
    }
};

struct Rectangle {
    Position pos{};
    Size size{};

    Rectangle() = default;
    constexpr Rectangle(int x, int y, int width, int height) noexcept : pos{x, y}, size{width, height} {}
    constexpr Rectangle(Position pos, Size size) noexcept : pos{pos}, size{size} {}
    constexpr auto x2() const noexcept -> int { return pos.x + size.width; }
    constexpr auto y2() const noexcept -> int { return pos.y + size.height; }
    constexpr auto bottomRight() const noexcept -> Position { return {x2(), y2()}; }
    [[nodiscard]] auto center() const noexcept -> Position {
        return {size.width / 2 + pos.x, size.height / 2 + pos.y};
    }
    auto operator|=(const Rectangle &other) noexcept -> Rectangle& {
        auto newPos1 = pos.componentMin(other.pos);
        auto newPos2 = bottomRight().componentMax(other.bottomRight());
        pos = newPos1;
        size = Size{newPos1, newPos2};
        return *this;
    }
    [[nodiscard]] auto padded(int paddingX, int paddingY) const noexcept -> Rectangle {
        return Rectangle{pos.x - paddingX, pos.y - paddingY, size.width + paddingX * 2, size.height + paddingY * 2};
    }
    [[nodiscard]] auto contains(const Position& testedPosition) const noexcept -> bool {
        return testedPosition.x >= pos.x && testedPosition.y >= pos.y
            && testedPosition.x < x2() && testedPosition.y < y2();
    }
    template<typename Fn> void forEach(Fn fn) const {
        for (int y = 0; y < size.height; ++y) {
            for (int x = 0; x < size.width; ++x) {
                fn(Position{x, y} + pos);
            }
        }
    }
};
