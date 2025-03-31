#include "gbz-tool.h"
#include <iostream>

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <command> [arguments]" << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << "  info <input.gbz>              Display information about a GBZ file" << std::endl;
    std::cerr << "  convert <input.gfa> <output.gbz> Convert GFA file to GBZ format" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string command = argv[1];
    GBZTool tool;

    if (command == "info" && argc == 3) {
        tool.process_gbz(argv[2]);
    } else if (command == "convert" && argc == 4) {
        tool.convert_gfa_to_gbz(argv[2], argv[3]);
    } else {
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}