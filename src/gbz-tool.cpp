#include "gbz-tool.h"
#include <gbwtgraph/utils.h>
#include <iostream>
#include <string>

GBZTool::GBZTool() : gbz_graph(nullptr) {}

GBZTool::~GBZTool() {
    if (gbz_graph) {
        delete gbz_graph;
    }
}

bool GBZTool::load_gbz(const std::string& filename) {
    try {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) {
            std::cerr << "Error: Could not open GBZ file: " << filename << std::endl;
            return false;
        }
        gbz.simple_sds_load(in);
        in.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading GBZ file: " << e.what() << std::endl;
        return false;
    }
}

void GBZTool::process_gbz(const std::string& filename) {
    try {
        // Load the GBZ file
        gbwtgraph::GBZ temp_gbz;
        std::ifstream in(filename, std::ios_base::binary);
        size_t bytes = gbwt::fileSize(in);

        temp_gbz.simple_sds_load(in);
        in.close();

        std::cout << filename << " loaded!" << std::endl;

        std::cout << "File size: " << bytes << " bytes" << std::endl;
        std::cout << "Number of nodes: " << temp_gbz.graph.get_node_count() << std::endl;
        std::cout << "Number of edges: " << temp_gbz.graph.get_edge_count() << std::endl;
        std::cout << "GBWT size: " << temp_gbz.index.size() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error processing GBZ file: " << e.what() << std::endl;
    }
}

void GBZTool::convert_gfa_to_gbz(const std::string& gfa_filename, const std::string& gbz_output_filename) {
    try {
        if (gbz_graph) {
            delete gbz_graph;
        }
        auto gfa_parse = gbwtgraph::gfa_to_gbwt(gfa_filename);
        gbz_graph = new gbwtgraph::GBZ(*(gfa_parse.first), *(gfa_parse.second));

        std::ofstream out_file(gbz_output_filename, std::ios::out | std::ios::binary);
        if (!out_file.is_open()) {
            throw std::runtime_error("Could not open output file: " + gbz_output_filename);
        }
        gbz_graph->simple_sds_serialize(out_file);
        out_file.close();
        std::cout << "Conversion completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error converting GFA to GBZ: " << e.what() << std::endl;
    }
}

std::string GBZTool::find_node_info(const std::string& gbz_file, gbwt::node_type node_id) {
    try {
        gbwtgraph::GBZ local_gbz;
        std::ifstream in(gbz_file, std::ios::binary);
        if (!in.is_open()) {
            return "Error: Could not open GBZ file: " + gbz_file;
        }
        local_gbz.simple_sds_load(in);
        in.close();
        const gbwtgraph::GBWTGraph& graph = local_gbz.graph;

        if (!graph.has_node(node_id)) {
            return "Error: Node " + std::to_string(node_id) + " does not exist.";
        }

        gbwtgraph::handle_t handle = graph.get_handle(node_id, false);
        std::string sequence = graph.get_sequence(handle);

        if (sequence.empty()) {
            return "Error: Node " + std::to_string(node_id) + " has no sequence.";
        }

        return "Node " + std::to_string(node_id) + ": [" + sequence + "]";
    } catch (const std::exception& e) {
        return "Error processing GBZ file: " + std::string(e.what());
    }
}

void GBZTool::find_interactive(const std::string& gbz_file) {
    if (!load_gbz(gbz_file)) {
        std::cerr << "Failed to load GBZ file. Exiting interactive mode." << std::endl;
        return;
    }

    std::cout << "Loaded " << gbz_file << ". Enter node ID to query (or 'exit' to quit):" << std::endl;
    std::string input;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            std::cout << "Exiting interactive mode." << std::endl;
            break;
        }

        try {
            gbwt::node_type node_id = std::stoul(input);
            const gbwtgraph::GBWTGraph& graph = gbz.graph;

            if (!graph.has_node(node_id)) {
                std::cout << "Error: Node " << node_id << " does not exist." << std::endl;
                continue;
            }

            gbwtgraph::handle_t handle = graph.get_handle(node_id, false);
            std::string sequence = graph.get_sequence(handle);

            if (sequence.empty()) {
                std::cout << "Error: Node " << node_id << " has no sequence." << std::endl;
                continue;
            }

            std::cout << "Node " << node_id << ": [" << sequence << "]" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid node ID: " << input << std::endl;
        }
    }
}