#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <optional>
#include <cstdint>
#include <string>
#include <vector>

struct CommandLineParameters
{
    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    std::string sdlRenderer;
    bool fullscreen = false;
    std::string language;
    bool enableCheats = false;
    std::vector<uint32_t> level;
};

std::optional<CommandLineParameters> parseCommandLine(int argc, char* argv[]);

#endif