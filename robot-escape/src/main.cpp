#include <erbsland/all_conf.hpp>

#include <filesystem>
#include <iostream>
#include <format>

using namespace el::conf;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <config-file>\n";
        return 1;
    }
    const auto configFile = std::filesystem::path{argv[1]};
    try {
        Parser parser;
        const auto source = Source::fromFile(configFile);
        const auto doc = parser.parseOrThrow(source);
        auto width = doc->getIntegerOrThrow(u8"field.width");
        auto height = doc->getIntegerOrThrow(u8"field.height");
        std::cout << std::format("Field width = {}, height = {}\n", width, height);
        return 0;
    } catch (const Error &error) {
        std::cerr << error.toText().toCharString() << "\n";
        return 1;
    }
}


