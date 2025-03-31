#ifndef HANDLEGRAPH_ALGORITHMS_INTERNAL_DFS_HPP_INCLUDED
#define HANDLEGRAPH_ALGORITHMS_INTERNAL_DFS_HPP_INCLUDED

#include "handlegraph/handle_graph.hpp"

#include <unordered_set>
#include <vector>

namespace handlegraph {
namespace algorithms {
namespace internal {

using namespace std;

// TODO: I don't love this DFS implementation, which is too confusing to
// be all that repurposeable. Hiding it in "internal" for now.

/// Depth first search across node traversals with interface to traversal tree via callback
void dfs(
         const HandleGraph& graph,
         const function<void(const handle_t&)>& handle_begin_fn,  // called when node orientation is first encountered
         const function<void(const handle_t&)>& handle_end_fn,    // called when node orientation goes out of scope
         const function<bool(void)>& break_fn,                    // called to check if we should stop the DFS; we stop when true is returned.
         const function<void(const edge_t&)>& edge_fn,            // called when an edge is encountered
         const function<void(const edge_t&)>& tree_fn,            // called when an edge forms part of the DFS spanning tree
         const function<void(const edge_t&)>& edge_curr_fn,       // called when we meet an edge in the current tree component
         const function<void(const edge_t&)>& edge_cross_fn,      // called when we meet an edge in an already-traversed tree component
         const vector<handle_t>& sources,                         // start only at these node traversals
         const unordered_set<handle_t>& sinks                     // when hitting a sink, don't keep walking
);

void dfs(const HandleGraph& graph,
         const function<void(const handle_t&)>& handle_begin_fn,
         const function<void(const handle_t&)>& handle_end_fn,
         const vector<handle_t>& sources,
         const unordered_set<handle_t>& sinks);
         
void dfs(const HandleGraph& graph,
         const function<void(const handle_t&)>& handle_begin_fn,
         const function<void(const handle_t&)>& handle_end_fn,
         const function<bool(void)>& break_fn);

}
}
}

#endif
