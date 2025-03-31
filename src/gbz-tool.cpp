#include "gbz-tool.h"
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

        // // Print basic information
        // std::cout << "GBZ File Information:" << std::endl;
        // std::cout << "---------------------" << std::endl;
        std::cout << "File size: " << bytes << " bytes" << std::endl;

        // // Print number of nodes
        std::cout << "Number of nodes: " << gbz.graph.get_node_count() << std::endl;
        
        // // Print number of edges
        std::cout << "Number of edges: " << gbz.graph.get_edge_count() << std::endl;
        
        // // Print GBWT size
        std::cout << "GBWT size: " << gbz.index.size() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error processing GBZ file: " << e.what() << std::endl;
    }
}

void GBZTool::convert_gfa_to_gbz(const std::string& gfa_filename, const std::string& gbz_output_filename) {
    try {
        // Create a new GBZ object
        if (gbz_graph) {
            delete gbz_graph;
        }
        
        auto gfa_parse = gbwtgraph::gfa_to_gbwt(gfa_filename);
        // GBWTGraph graph(*(gfa_parse.first), *(gfa_parse.second));
        gbz_graph = new gbwtgraph::GBZ(*(gfa_parse.first), *(gfa_parse.second));

        // // Open GFA file
        // std::ifstream gfa_file(gfa_filename, std::ios::in);
        // if (!gfa_file.is_open()) {
        //     throw std::runtime_error("Could not open GFA file: " + gfa_filename);
        // }

        // // Convert GFA to GBZ
        // std::cout << "Converting " << gfa_filename << " to " << gbz_output_filename << "..." << std::endl;
        // gbwtgraph::gfa_to_gbz(gfa_file, *gbz_graph);

        // // Save the GBZ file
        std::ofstream out_file(gbz_output_filename, std::ios::out | std::ios::binary);
        if (!out_file.is_open()) {
            throw std::runtime_error("Could not open output file: " + gbz_output_filename);
        }
        gbz_graph->simple_sds_serialize(out_file);
        out_file.close();

        std::cout << "Conversion completed successfully" << std::endl;

        // gfa_file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error converting GFA to GBZ: " << e.what() << std::endl;
    }
}