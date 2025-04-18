#include "gbz-tool.h"
#include <gbwtgraph/utils.h>
#include <nlohmann/json.hpp> // Assuming use of nlohmann/json library
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

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

            // get the string that contains node info in json format
            std::string s_node_info = this->_get_node_info(gbz.index, gbz.graph, node_id);
            std::cout << s_node_info << std::endl;
            
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


std::string GBZTool::_get_node_info(const gbwt::GBWT& gbwt_index, 
                            const gbwtgraph::GBWTGraph& graph, 
                            gbwt::node_type node_id) {
    json node_info;
    node_info["node_id"] = node_id;

    // Get node handle
    gbwtgraph::handle_t node_handle = graph.get_handle(node_id, false);

    // Get sequence
    std::string sequence = graph.get_sequence(node_handle);
    node_info["sequence"] = sequence;
    node_info["sequence_length"] = sequence.length();

    // Get in-edges
    json in_edges = json::array();
    graph.follow_edges(node_handle, true, [&](gbwtgraph::handle_t prev) {
        json edge;
        edge["from"] = graph.get_id(prev);
        edge["from_orientation"] = graph.get_is_reverse(prev) ? "-" : "+";
        edge["to"] = node_id;
        edge["to_orientation"] = "+";
        in_edges.push_back(edge);
    });
    node_info["in_edges"] = in_edges;

    // Get out-edges
    json out_edges = json::array();
    graph.follow_edges(node_handle, false, [&](gbwtgraph::handle_t next) {
        json edge;
        edge["from"] = node_id;
        edge["from_orientation"] = "+";
        edge["to"] = graph.get_id(next);
        edge["to_orientation"] = graph.get_is_reverse(next) ? "-" : "+";
        out_edges.push_back(edge);
    });
    node_info["out_edges"] = out_edges;

    // Find haplotypes containing the node
    json haplotypes = json::array();
    gbwt::node_type node = gbwt::Node::encode(node_id, false);
    gbwt::SearchState search_state = gbwt_index.find(node);
    std::vector<gbwt::size_type> haplotype_ids = gbwt_index.locate(search_state);

    if (!haplotype_ids.empty()) {
        // Get reference samples for path sense determination
        auto reference_samples = gbwtgraph::parse_reference_samples_tag(gbwt_index);

        // For each haplotype containing this node
        for (size_t i = 0; i < haplotype_ids.size(); i++) {
            json haplotype;
            auto haplotype_id = haplotype_ids[i];
            haplotype["haplotype_number"] = i + 1;
            haplotype["haplotype_id"] = haplotype_id;

            // Get path information
            gbwt::size_type path_id = gbwt::Path::id(haplotype_id);

            if (gbwt_index.hasMetadata()) {
                // Get path metadata
                gbwtgraph::PathSense sense = gbwtgraph::get_path_sense(gbwt_index, path_id, reference_samples);
                haplotype["chromosome"] = gbwtgraph::get_path_sample_name(gbwt_index, path_id, sense);
                haplotype["region"] = gbwtgraph::get_path_locus_name(gbwt_index, path_id, sense);
                haplotype["haplotype_number"] = gbwtgraph::get_path_haplotype(gbwt_index, path_id, sense);
                
                // Set path type
                switch(sense) {
                    case gbwtgraph::PathSense::REFERENCE:
                        haplotype["path_type"] = "Reference";
                        break;
                    case gbwtgraph::PathSense::HAPLOTYPE:
                        haplotype["path_type"] = "Haplotype";
                        break;
                    case gbwtgraph::PathSense::GENERIC:
                        haplotype["path_type"] = "Generic";
                        break;
                    default:
                        haplotype["path_type"] = "Unknown";
                }

                // Get thread information
                gbwt::vector_type thread = gbwt_index.extract(haplotype_id);
                haplotype["thread_length"] = thread.size();

                // Get thread context
                json thread_context = json::array();
                bool found_node = false;
                for (size_t j = 0; j < thread.size(); j++) {
                    if (gbwt::Node::id(thread[j]) == node_id) {
                        found_node = true;
                        // Collect 2 nodes before and after if available
                        for (int k = std::max(0, (int)j - 2); k <= std::min((int)thread.size() - 1, (int)j + 2); k++) {
                            json node;
                            node["id"] = gbwt::Node::id(thread[k]);
                            node["is_target"] = (k == (int)j);
                            thread_context.push_back(node);
                        }
                        break;
                    }
                }
                haplotype["thread_context"] = found_node ? thread_context : "Node not found in thread";
            } else {
                haplotype["metadata"] = "No metadata available";
            }
            haplotypes.push_back(haplotype);
        }
    }
    node_info["haplotypes"] = haplotypes;
    node_info["haplotype_count"] = haplotypes.size();

    // Construct final string
    std::string s_node_info = std::to_string(node_id) + ": " + node_info.dump(2);
    
    return s_node_info;
}