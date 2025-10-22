#include "CLI11.hpp"
#include "simplecpp.h"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

#pragma comment(lib, "Shell32")

int main(int argc, char** argv)
{
    const fs::path launchCppFront = (fs::current_path() / "cppfront.exe");
    const fs::path launchClangFmt = (fs::current_path() / "clang-format.exe");

    if (!fs::exists(launchCppFront)) {
        std::printf("Error; could not locate cppfront, expected at \"%s\"\n", launchCppFront.string().c_str());
        exit(-1);
    }
    if (!fs::exists(launchClangFmt)) {
        std::printf("Error; Could not locate clang-format, expected at \"%s\"\n", launchClangFmt.string().c_str());
        exit(-1);
    }

    // ------------------------------

    CLI::App app { "Wrapper for executing cppfront, clang-format, etc via MSBuild integration" };
    argv = app.ensure_utf8(argv);

    std::string argInputfile = "main.cpp2";
    app.add_option("--file", argInputfile, ".cpp2/.h2 input file");

    std::string argRelativeDir = "";
    app.add_option("--reldir", argRelativeDir, "relative directory to input");

    std::string argFormatStyle = "WebKit";
    app.add_option("--style", argFormatStyle, "style choice passed to clang-format (WebKit default)");

    bool argVerboseDispatch = false;
    app.add_option("--verbose", argVerboseDispatch, "log out debug stuff during dispatch");

    // add a subcommand that captures everything after "--cpf" and will forward it all to cppfront directly
    auto* subcom = app.add_subcommand("cpf", "all subsequent arguments beyond this point forwarded to cppfront")->prefix_command();
    subcom->alias("--cpf");

    CLI11_PARSE(app, argc, argv);

    // ------------------------------

    std::string runCppfrontCmd = std::format("{0} -cwd {1} ", launchCppFront.string(), argRelativeDir);
    for (const auto& aarg : subcom->remaining()) {
        runCppfrontCmd += aarg;
        runCppfrontCmd += " ";
    }
    runCppfrontCmd += argInputfile;

    if (argVerboseDispatch)
        std::printf("launching \"%s\"\n", runCppfrontCmd.c_str());
    std::system(runCppfrontCmd.c_str());

    std::string outputFile = std::regex_replace(argInputfile, std::regex("\\.cpp2"), ".cpp");
                outputFile = std::regex_replace(outputFile, std::regex("\\.h2"), ".h");

    std::string runFormatCmd = std::format("{0} -i --style={1} {2}{3} ",
        launchClangFmt.string(),
        argFormatStyle,
        argRelativeDir,
        outputFile);

    if (argVerboseDispatch)
        std::printf("launching \"%s\"\n", runFormatCmd.c_str());
    std::system(runFormatCmd.c_str());

    // ------------------------------

#if 0
    const fs::path& fileIn = argInputfile;

    simplecpp::OutputList outputList;
    simplecpp::DUI dui;

    std::vector<std::string> files;
    std::ifstream f( fileIn );
    simplecpp::TokenList rawtokens( f, files, fileIn.string(), &outputList );

    std::map<std::string, simplecpp::TokenList*> included = simplecpp::load( rawtokens, files, dui, &outputList );

    for ( const auto& inc : included )
    {
    }
#endif

    return 0;
}