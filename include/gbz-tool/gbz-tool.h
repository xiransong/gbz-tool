#ifndef GBZ_TOOL_H
#define GBZ_TOOL_H

#include <gbwtgraph/gbz.h>
#include <gbwtgraph/gfa.h>

class GBZTool {
public:
    GBZTool();
    ~GBZTool();
    
    // Process GBZ file and print information
    void process_gbz(const std::string& filename);
    
    // Convert GFA to GBZ
    void convert_gfa_to_gbz(const std::string& gfa_filename, const std::string& gbz_output_filename);

    private:
    gbwtgraph::GBZ* gbz_graph;
};

#endif