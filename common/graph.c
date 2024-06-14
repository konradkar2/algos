#include "graph.h"
#include "../utils/da_append.h"
#include <assert.h>
#include <stdio.h>

struct node *graph_find_node(struct Graph *graph, int id) {
  for (size_t i = 0; i < graph->nodes.count; ++i) {
    struct node *node = &graph->nodes.items[i];
    if (node->id == id) {
      return node;
    }
  }
  return NULL;
}

void graph_destroy(struct Graph *graph) {
  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *current_edge = &graph->edges.items[i];
    current_edge->nodes[0] = NULL;
    current_edge->nodes[1] = NULL;
  }

  for (size_t i = 0; i < graph->nodes.count; ++i) {
    struct node *current_node = &graph->nodes.items[i];
    da_reset(&current_node->edges);
  }
}

void graph_construct(struct Graph *graph) {
  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *current_edge = &graph->edges.items[i];
    for (int j = 0; j < 2; ++j) {
      struct node *node = graph_find_node(graph, current_edge->node_ids[j]);
      assert(node != NULL && "edge has assigned unexisting node id");
      da_append_safe(&node->edges, current_edge);
      current_edge->nodes[j] = node;
    }
  }
}

void graph_copy(const struct Graph *in, struct Graph *output) {
  for (size_t i = 0; i < in->edges.count; ++i) {
    struct edge *current_edge = &in->edges.items[i];
    da_append_safe(&output->edges, *current_edge);
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
  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *edge = &graph->edges.items[i];
    printf("edge (id: %d, drawn: %d), nodes: (%d visited: %d, %d, visited: %d)\n",
           edge->id, edge->visited, edge->nodes[0]->id, edge->nodes[0]->visited,
           edge->nodes[1]->id, edge->nodes[1]->visited);
  }
}
