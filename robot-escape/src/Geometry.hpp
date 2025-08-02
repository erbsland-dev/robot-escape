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
};

constexpr static std::array<Position, 4> cPosDelta4 = {
    {{1, 0}, {0, 1}, {-1, 0}, {0, -1}}};

struct Size {
    int width{};
    int height{};

    Size() = default;
    constexpr Size(int width, int height) noexcept : width{width}, height{height} {}
    [[nodiscard]] auto fitsInto(Size other) const noexcept -> bool {
        return width <= other.width && height <= other.height;
    }
    [[nodiscard]] auto area() const noexcept -> int { return width * height; }
    [[nodiscard]] auto center() const noexcept -> Position {
        return {width / 2, height / 2};
    }
    [[nodiscard]] auto contains(const Position& pos) const noexcept -> bool {
        return pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height;
    }
    [[nodiscard]] auto index(const Position& pos) const noexcept -> int {
        return pos.y * width + pos.x;
    }
    [[nodiscard]] auto randomPosition() const noexcept -> Position {
        return {randomInt(0, width - 1), randomInt(0, height - 1)};
    }
    template<typename Fn> void forEach(Fn fn) const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                fn(Position{x, y});
            }
        }
    }
    template<typename Fn> auto filterPositions(Fn fn) const -> std::vector<Position> {
        std::vector<Position> result;
        result.reserve(area());
        forEach([&result, &fn](const Position& pos) {
            if (fn(pos)) { result.push_back(pos); }
        });
        return result;
    }
};
