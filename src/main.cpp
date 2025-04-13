#include "gbz-tool.h"
#include <iostream>

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <command> [arguments]" << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << "  info <input.gbz>              Display information about a GBZ file" << std::endl;
    std::cerr << "  convert <input.gfa> <output.gbz> Convert GFA file to GBZ format" << std::endl;
    std::cerr << "  find <input.gbz> <node_id>    Find node sequence for given node ID" << std::endl;
    std::cerr << "  find-inter <input.gbz>        Interactive mode to find node sequences" << std::endl;
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
    } else if (command == "find" && argc == 4) {
        try {
            gbwt::node_type node_id = std::stoul(argv[3]);
            std::string result = tool.find_node_info(argv[2], node_id);
            std::cout << result << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid node ID: " << argv[3] << std::endl;
            return 1;
        }
    } else if (command == "find-inter" && argc == 3) {
        tool.find_interactive(argv[2]);
    } else {
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}