#pragma once

#include "Canvas.hpp"
#include "Logic.hpp"
#include "World.hpp"

#include <erbsland/all_conf.hpp>

#include <filesystem>
#include <iostream>

using el::conf::Parser;
using el::conf::Source;
using el::conf::DocumentPtr;
using el::conf::ValuePtr;
using el::conf::Error;

struct Application {
    std::filesystem::path configPath;
    DocumentPtr config;

    constexpr static auto cMinimumFieldSize = Size{8, 8};
    constexpr static auto cMaximumFieldSize = Size{80, 40};
    constexpr static auto cMinimumCanvasSize = Size{32, 16};

    void parseArgs(int argc, char **argv) {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " <config-file>\n";
            exit(1);
        }
        configPath = std::filesystem::path{argv[1]};
    }

    void readConfiguration() {
        try {
            Parser parser;
            const auto source = Source::fromFile(configPath);
            config = parser.parseOrThrow(source);
        } catch (const Error &error) {
            std::cerr << error.toText().toCharString() << "\n";
            exit(1);
        }
    }

    [[noreturn]] static void exitWithErrorInValue(const ValuePtr &value, const std::string &message) {
        std::cerr << message
            << " For value '"
            << value->namePath().toText().toCharString()
            << "' at "
            << value->location().toText().toCharString();
        exit(1);
    }

    [[nodiscard]] static auto rectFromSection(const ValuePtr &value) -> Rectangle {
        Rectangle result;
        if (value->hasValue(u8"rectangle")) {
            const auto rectList = value->getListOrThrow<int>(u8"rectangle");
            if (rectList.size() != 4) { exitWithErrorInValue(value, "Rectangle must have exactly four elements."); }
            result = Rectangle(rectList[0], rectList[1], rectList[2], rectList[3]);
        } else if (value->hasValue(u8"position") != value->hasValue(u8"size")) {
            exitWithErrorInValue(value, "Only 'position' or 'size' is not allowed.");
        } else if (value->hasValue(u8"position") && value->hasValue(u8"size")) {
            auto posList = value->getListOrThrow<int>(u8"position");
            if (posList.size() != 2) { exitWithErrorInValue(value, "Position must have exactly two elements."); }
            auto sizeList = value->getListOrThrow<int>(u8"size");
            if (sizeList.size() != 2) { exitWithErrorInValue(value, "Size must have exactly two elements."); }
            result = Rectangle(Position{posList[0], posList[1]}, Size(sizeList[0], sizeList[1]));
        } else {
            result = Rectangle{value->getOrThrow<int>(u8"x"), value->getOrThrow<int>(u8"y"),
                value->getOrThrow<int>(u8"width"), value->getOrThrow<int>(u8"height")};
        }
        return result;
    }

    [[nodiscard]] auto buildWorld() const -> World {
        try {
            World world;
            for (const auto &roomValue : *config->getSectionListOrThrow("field.room")) {
                const auto roomRect = rectFromSection(roomValue);
                world.field.addRoom(roomRect);
            }
            if (!cMinimumFieldSize.fitsInto(world.field.rect.size)) {
                std::cerr << std::format("Field size must be at least {}x{}\n", cMinimumFieldSize.width, cMinimumFieldSize.height);
                exit(1);
            }
            if (!world.field.rect.size.fitsInto(cMaximumFieldSize)) {
                std::cerr << std::format("Field size must be at most {}x{}\n", cMaximumFieldSize.width, cMaximumFieldSize.height);
                exit(1);
            }
            world.addExitAtRandomPosition();
            world.setPlayerToRandomPosition();
            for (int i = 0; i < 3; ++i) {
                world.addRobotAtRandomPosition();
            }
            return world;
        } catch (const Error &error) {
            std::cerr << error.toText().toCharString() << "\n";
            exit(1);
        }
    }

    void renderLogic(const Logic &logic) {
        auto canvasSize = logic.world.field.rect.padded(2, 1).size.componentMax(cMinimumCanvasSize);
        Canvas canvas{canvasSize};
        logic.render(canvas);
        canvas.renderToConsole();
    }

    void run() {
        auto initialWorld = buildWorld();
        std::cout << "----------------------------==[ ROBOT ESCAPE ]==-----------------------------\n";
        std::cout << "Welcome to Robot Escape!\n";
        std::cout << "You (☻) must run to the exit (⚑) before any robot (♟) catches you.\n\n";
        auto logic = Logic{std::move(initialWorld)};
        renderLogic(logic);
        auto state = logic.gameState();
        while (state == GameState::Running) {
            auto playerInput = inputFromConsole();
            if (playerInput == cQuitInput) {
                std::cout << "Goodbye!\n";
                return;
            }
            logic.advance(playerInput);
            renderLogic(logic);
            state = logic.gameState();
        }
        if (state == GameState::PlayerWon) {
            std::cout << "You won!\n";
        } else {
            std::cout << "You lost!\n";
        }
    }
};

