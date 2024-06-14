#include "graph_algos.h"
#include "graph.h"
#include <limits.h>

uint get_graph_smallest_deg(struct Graph *graph) {
  uint result = UINT_MAX;

  FOR_EACH_NODE_IN_GRAPH(graph, node_it) {
    const uint node_deg = get_node_deg(node_it);
    if (node_deg < result) {
      result = node_deg;
    }
  }
  return result;
}

uint get_node_deg(const struct node *node) { return node->edges.count; }

void visit_node(struct node *already_visited_node, struct node *node,
                struct edge *edge, dfs_callback callback) {
  node->visited = true;
  callback(already_visited_node, node, edge);

  FOR_EACH_EDGE_IN_NODE(node, edge_it) {
    struct node *neighbour = graph_get_neighbour_of(node, *edge_it);
    if (neighbour->visited == false) {
      visit_node(node, neighbour, *edge_it, callback);
    }
  }
}

void depth_first_search(struct Graph *graph, dfs_callback cb) {
  FOR_EACH_NODE_IN_GRAPH(graph, node_it) { node_it->visited = false; }

  FOR_EACH_NODE_IN_GRAPH(graph, node_it) {
    if (node_it->visited == false) {
      visit_node(NULL, node_it, NULL, cb);
    }
  }
}
