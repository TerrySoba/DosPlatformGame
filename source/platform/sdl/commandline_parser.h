#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <optional>
#include <cstdint>
#include <string>

struct CommandLineParameters
{
    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    std::string sdlRenderer;
    bool fullscreen = false;
    std::string language;
};

std::optional<CommandLineParameters> parseCommandLine(int argc, char* argv[]);

#endif