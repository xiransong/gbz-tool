#ifndef GBZ_TOOL_H
#define GBZ_TOOL_H

#include <gbwtgraph/gbz.h>
#include <gbwtgraph/gfa.h>
#include <gbwtgraph/utils.h>

class GBZTool {
public:
    GBZTool();
    ~GBZTool();
    
    // Process GBZ file and print information
    void process_gbz(const std::string& filename);
    
    // Convert GFA to GBZ
    void convert_gfa_to_gbz(const std::string& gfa_filename, const std::string& gbz_output_filename);
    
    // Find node information (sequence) given a node ID
    void find_node_info(const std::string& gbz_file, gbwt::node_type node_id);
    
    // Interactive node finding mode
    void find_interactive(const std::string& gbz_file);
    
    // Batch node finding mode
    void find_batch(const std::string& gbz_file, const std::string& node_ids_file, const std::string& output_json);

    std::string _get_node_info(const gbwt::GBWT& gbwt_index, const gbwtgraph::GBWTGraph& graph, gbwt::node_type node_id);

private:
    // Load GBZ file into the member variable
    bool load_gbz(const std::string& filename);
    
    gbwtgraph::GBZ* gbz_graph; // For conversion operations
    gbwtgraph::GBZ gbz;        // Persistent GBZ for interactive and batch modes
};

#endif