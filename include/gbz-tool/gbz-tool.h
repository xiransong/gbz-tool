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
    std::string find_node_info(const std::string& gbz_file, gbwt::node_type node_id);
    
    // Interactive node finding mode
    void find_interactive(const std::string& gbz_file);

private:
    // Load GBZ file into the member variable
    bool load_gbz(const std::string& filename);
    
    gbwtgraph::GBZ* gbz_graph; // For conversion operations
    gbwtgraph::GBZ gbz;        // Persistent GBZ for interactive mode
};

#endif