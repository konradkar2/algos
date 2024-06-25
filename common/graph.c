#include "graph.h"
#include "../utils/da_append.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

struct node *graph_find_node(struct Graph *graph, int id) {
  FOR_EACH_NODE_IN_GRAPH(graph, node_it) {
    if (node_it->id == id) {
      return node_it;
    }
  }
  return NULL;
}

void graph_add_node(struct Graph *graph, int node_id) {
  da_append_safe(&graph->nodes, (struct node){.id = node_id});
}

void graph_add_edge(struct Graph *graph, int node_a_id, int node_b_id) {
  const size_t id = graph->edges.count;
  da_append_safe(&graph->edges,
                 ((struct edge){.node_ids = {node_a_id, node_b_id}, .id = id}));
}

void graph_destroy(struct Graph *graph) {
  FOR_EACH_EDGE_IN_GRAPH(graph, edge_it) {
    edge_it->nodes[0] = NULL;
    edge_it->nodes[1] = NULL;
  }

  for (size_t i = 0; i < graph->nodes.count; ++i) {
    struct node *current_node = &graph->nodes.items[i];
    da_reset(&current_node->edges);
  }
}

void graph_construct(struct Graph *graph) {
  FOR_EACH_EDGE_IN_GRAPH(graph, edge_it) {
    for (int j = 0; j < 2; ++j) {
      struct node *node = graph_find_node(graph, edge_it->node_ids[j]);
      if (node != NULL) {
        graph_dump(graph);
        assert(0 && "edge has assigned unexisting node id");
      }
      da_append_safe(&node->edges, edge_it);
      edge_it->nodes[j] = node;
    }
  }
}

void graph_copy(const struct Graph *in, struct Graph *output) {
  FOR_EACH_EDGE_IN_GRAPH(in, edge_it) {
    da_append_safe(&output->edges, *edge_it);
  }

  for (size_t i = 0; i < in->nodes.count; ++i) {
    struct node *current_node = &in->nodes.items[i];
    da_append_safe(&output->nodes, *current_node);
  }
  graph_destroy(output);
  graph_construct(output);
}

struct node *graph_get_neighbour_of(struct node *node, struct edge *edge) {
  if (edge->nodes[0]->id == node->id) {
    return edge->nodes[1];
  } else {
    return edge->nodes[0];
  }
}

void graph_dump(struct Graph *graph) {
  FOR_EACH_EDGE_IN_GRAPH(graph, edge_it) {
    printf(
        "edge (id: %d, drawn: %d), nodes: (%d visited: %d, %d, visited: %d)\n",
        edge_it->id, edge_it->visited, edge_it->nodes[0]->id,
        edge_it->nodes[0]->visited, edge_it->nodes[1]->id,
        edge_it->nodes[1]->visited);
  }
}
