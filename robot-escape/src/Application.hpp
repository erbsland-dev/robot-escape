#pragma once

#include "Canvas.hpp"
#include "Logic.hpp"
#include "World.hpp"

#include <erbsland/all_conf.hpp>

#include <filesystem>
#include <iostream>

using namespace el::conf;

struct Application {
    std::filesystem::path configPath;
    DocumentPtr config;
    Size canvasSize = Canvas::cDefaultSize;

    constexpr static auto cMinimumSize = Size{10, 10};

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

    [[nodiscard]] auto buildWorld() const -> World {
        try {
            auto fieldSize = Size{
                static_cast<int>(config->getIntegerOrThrow(u8"field.width")),
                static_cast<int>(config->getIntegerOrThrow(u8"field.height"))};
            if (!cMinimumSize.fitsInto(fieldSize)) {
                std::cerr << std::format("Field size must be at least {}x{}\n", cMinimumSize.width, cMinimumSize.height);
                exit(1);
            }
            if (!fieldSize.fitsInto(canvasSize)) {
                std::cerr << std::format("Field size must be at most {}x{}\n", canvasSize.width, canvasSize.height);
                exit(1);
            }
            World world{fieldSize};
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

