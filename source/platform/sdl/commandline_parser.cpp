#include "commandline_parser.h"

#include <CLI/CLI.hpp>

std::optional<CommandLineParameters> parseCommandLine(int argc, char* argv[])
{
    CLI::App app{"SDL Platform Game"};

    CommandLineParameters params;

    app.add_option("-x,--width", params.screenWidth, "Screen Width")->default_val(1920);
    app.add_option("-y,--height", params.screenHeight, "Screen Height")->default_val(1080);
    app.add_option("-r,--renderer", params.sdlRenderer, "SDL Renderer. Use 'list' to see a list of available renderers");
    app.add_flag("-f,--fullscreen", params.fullscreen, "Enable fullscreen mode");
    app.add_option("-l,--language", params.language, "Language to use.")->default_val("english");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError &e)
    {
        app.exit(e);
        return std::nullopt;
    }

    return params;
}