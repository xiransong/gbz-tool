#include "gbz-tool.h"
#include <gbwtgraph/utils.h>
#include <iostream>

GBZTool::GBZTool() : gbz_graph(nullptr) {}

GBZTool::~GBZTool() {
    if (gbz_graph) {
        delete gbz_graph;
    }
}

void GBZTool::process_gbz(const std::string& filename) {
    try {
        // Load the GBZ file
        gbwtgraph::GBZ gbz;
        std::ifstream in(filename, std::ios_base::binary);
        size_t bytes = gbwt::fileSize(in);

        gbz.simple_sds_load(in);
        in.close();

        std::cout << filename << " loaded!" << std::endl;

        std::cout << "File size: " << bytes << " bytes" << std::endl;
        std::cout << "Number of nodes: " << gbz.graph.get_node_count() << std::endl;
        std::cout << "Number of edges: " << gbz.graph.get_edge_count() << std::endl;
        std::cout << "GBWT size: " << gbz.index.size() << std::endl;

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

        // Debug: Print node sequences
        // std::cerr << "Debug: Nodes after conversion:\n";
        // gbz_graph->graph.for_each_handle([&](const gbwtgraph::handle_t& handle) {
        //     gbwt::node_type node_id = gbz_graph->graph.get_id(handle);
        //     std::string seq = gbz_graph->graph.get_sequence(handle);
        //     std::cerr << "Node " << node_id << ": " << seq << "\n";
        // });

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
        gbwtgraph::GBZ gbz;
        std::ifstream in(gbz_file, std::ios::binary);
        if (!in.is_open()) {
            return "Error: Could not open GBZ file: " + gbz_file;
        }
        gbz.simple_sds_load(in);
        in.close();
        const gbwtgraph::GBWTGraph& graph = gbz.graph;

        std::cerr << "Debug: Graph has " << graph.get_node_count() << " nodes\n";
        std::cerr << "Debug: Node ID range [" << graph.min_node_id() << ", " << graph.max_node_id() << "]\n";

        // Debug: Dump all nodes
        // std::cerr << "Debug: All nodes in graph:\n";
        // graph.for_each_handle([&](const gbwtgraph::handle_t& handle) {
        //     gbwt::node_type id = graph.get_id(handle);

        //     // gbwtgraph::handle_t handle_ = graph.node_to_handle(id);   // If use this, the output is not right!!!
        //     // std::string seq = graph.get_sequence(handle_);

        //     std::string seq = graph.get_sequence(handle);     // The output is correct!

        //     std::cerr << "Node " << id << ": '" << seq << "'\n";
        // });

        if (!graph.has_node(node_id)) {
            return "Error: Node " + std::to_string(node_id) + " does not exist.";
        }

        // gbwtgraph::handle_t handle = graph.node_to_handle(node_id);  // This is wrong!!!!!
        gbwtgraph::handle_t handle = graph.get_handle(node_id, false);  // This is correct!!!!!
        // std::cerr << "Debug: Node " << node_id << " maps to handle\n";

        std::string sequence = graph.get_sequence(handle);
        // std::cerr << "Debug: Raw sequence for node " << node_id << ": '" << sequence << "'\n";

        if (sequence.empty()) {
            return "Error: Node " + std::to_string(node_id) + " has no sequence.";
        }

        return "Node " + std::to_string(node_id) + ": [" + sequence + "]";
    } catch (const std::exception& e) {
        return "Error processing GBZ file: " + std::string(e.what());
    }
}