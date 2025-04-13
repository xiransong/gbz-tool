#include "gbz-tool.h"
#include <gbwtgraph/utils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

void GBZTool::find_batch(const std::string& gbz_file, const std::string& node_ids_file, const std::string& output_json) {
    // Load GBZ file
    if (!load_gbz(gbz_file)) {
        std::cerr << "Failed to load GBZ file. Exiting batch mode." << std::endl;
        return;
    }

    // Read node IDs from file
    std::ifstream ids_in(node_ids_file);
    if (!ids_in.is_open()) {
        std::cerr << "Error: Could not open node IDs file: " << node_ids_file << std::endl;
        return;
    }

    std::vector<gbwt::node_type> node_ids;
    std::string line;
    while (std::getline(ids_in, line)) {
        try {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) continue; // Skip empty lines
            gbwt::node_type node_id = std::stoul(line);
            node_ids.push_back(node_id);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid node ID in file: " << line << ". Skipping." << std::endl;
        }
    }
    ids_in.close();

    if (node_ids.empty()) {
        std::cerr << "Error: No valid node IDs found in " << node_ids_file << std::endl;
        return;
    }

    // Query node information
    std::vector<std::pair<gbwt::node_type, std::string>> results;
    const gbwtgraph::GBWTGraph& graph = gbz.graph;
    for (gbwt::node_type node_id : node_ids) {
        std::string result;
        if (!graph.has_node(node_id)) {
            result = "Error: Node " + std::to_string(node_id) + " does not exist.";
        } else {
            gbwtgraph::handle_t handle = graph.get_handle(node_id, false);
            std::string sequence = graph.get_sequence(handle);
            if (sequence.empty()) {
                result = "Error: Node " + std::to_string(node_id) + " has no sequence.";
            } else {
                result = sequence;
            }
        }
        results.emplace_back(node_id, result);
    }

    // Write JSON output
    std::ofstream out_json(output_json);
    if (!out_json.is_open()) {
        std::cerr << "Error: Could not open output JSON file: " << output_json << std::endl;
        return;
    }

    out_json << "{\n";
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& [node_id, info] = results[i];
        // Escape quotes in info string
        std::string escaped_info = info;
        size_t pos = 0;
        while ((pos = escaped_info.find('"', pos)) != std::string::npos) {
            escaped_info.replace(pos, 1, "\\\"");
            pos += 2;
        }
        out_json << "  \"" << node_id << "\": \"" << escaped_info << "\"";
        if (i < results.size() - 1) {
            out_json << ",";
        }
        out_json << "\n";
    }
    out_json << "}\n";
    out_json.close();

    std::cout << "Batch processing completed. Output written to " << output_json << std::endl;
}