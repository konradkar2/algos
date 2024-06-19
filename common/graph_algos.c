#include "graph_algos.h"
#include "graph.h"
#include <limits.h>
#include <time.h>

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
  bool callback_ret = callback(udata, node_from, node_to, edge);

  if (edge) {
    edge->visited = true;
  }
  node_to->visited = true;

  if (callback_ret == false) {
    return;
  }

  FOR_EACH_EDGE_IN_NODE(node_to, edge_it) {
    struct node *neighbour = graph_get_neighbour_of(node_to, *edge_it);
    if (neighbour->visited == false) {
      dfs_visit_node(node_to, neighbour, *edge_it, udata, callback);
    }
  }
}

void dfs_visit_node_and_edge(struct node *node_from, struct node *node_to,
                             struct edge *edge, void *udata,
                             dfs_callback callback) {
  bool callback_ret = callback(udata, node_from, node_to, edge);

  if (edge) {
    edge->visited = true;
  }
  node_to->visited = true;

  if (callback_ret == false) {
    return;
  }

  FOR_EACH_EDGE_IN_NODE(node_to, edge_it) {
    struct node *neighbour = graph_get_neighbour_of(node_to, *edge_it);
    if (neighbour->visited == false || (*edge_it)->visited == false) {
      dfs_visit_node_and_edge(node_to, neighbour, *edge_it, udata, callback);
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

vector_of(int, int_vec);

struct cycle_finder_state {
  struct int_vec traversed_nodes_ids;
  struct graph_vec result_graphs;
};

ssize_t find_idx_of_node(struct int_vec *vec, int node_id) {
  DA_FOR_EACH(vec, idx) {
    if (vec->items[idx] == node_id) {
      return idx;
    }
  }

  return -1;
}

bool find_cycle_cb(void *udata, struct node *from, struct node *to,
                   struct edge *edge) {
  (void)edge;

  struct cycle_finder_state *state = udata;
  // root node
  if (from == NULL) {
    da_append_safe(&state->traversed_nodes_ids, to->id);
  } else {
    ssize_t idx_in_traversed =
        find_idx_of_node(&state->traversed_nodes_ids, to->id);
    // found cycle
    if (idx_in_traversed != -1) {
      struct Graph cycle_graph = {0};
      int *traversed_nodes = state->traversed_nodes_ids.items;
      for (size_t i = idx_in_traversed; i < state->traversed_nodes_ids.count;
           ++i) {

        if (i != state->traversed_nodes_ids.count - 1) {
          graph_add_node(&cycle_graph, traversed_nodes[i]);
        }

        if (i != (size_t)idx_in_traversed) {
          graph_add_edge(&cycle_graph, traversed_nodes[i],
                         traversed_nodes[i - 1]);
        }
      }
      da_append_safe(&state->result_graphs, cycle_graph);
      da_reset(&state->traversed_nodes_ids);
    }
  }

  return true;
}

struct graph_vec find_cycles(const struct Graph *graph,
                             struct node *root_node) {
  (void)graph;                                
  struct cycle_finder_state state = {0};
  dfs_visit_node_and_edge(NULL, root_node, NULL, &state, find_cycle_cb);

  return state.result_graphs;
}
