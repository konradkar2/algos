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

void dfs_visit_node(struct node *node_from, struct node *node_to,
                    struct edge *edge, void *udata, dfs_callback callback) {
  if (edge) {
    edge->visited = true;
  }

  node_to->visited = true;
  if (callback(udata, node_from, node_to, edge) == false) {
    return;
  }

  FOR_EACH_EDGE_IN_NODE(node_to, edge_it) {
    struct node *neighbour = graph_get_neighbour_of(node_to, *edge_it);
    if (neighbour->visited == false) {
      dfs_visit_node(node_to, neighbour, *edge_it, udata, callback);
    }
  }
}

void depth_first_search(struct Graph *graph, void *udata, dfs_callback cb) {
  FOR_EACH_NODE_IN_GRAPH(graph, node_it) { node_it->visited = false; }

  FOR_EACH_NODE_IN_GRAPH(graph, node_it) {
    if (node_it->visited == false) {
      dfs_visit_node(NULL, node_it, NULL, udata, cb);
    }
  }
}
