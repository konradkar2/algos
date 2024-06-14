#ifndef GRAPH_H
#define GRAPH_H

#include "../utils/da_append.h"
#include <assert.h>
#include <raylib.h>
#include <stdbool.h>
#include <unistd.h>

struct node;

struct edge {
  int id;
  int node_ids[2];
  // populated later
  struct node *nodes[2];
  bool visited;
};

vector_of(struct edge, edge_vec);
vector_of(struct edge *, ref_edge_vec);

struct node {
  int id;
  // populated later
  bool visited;
  struct ref_edge_vec edges;
  // drawing
  Vector2 pos;
};

vector_of(struct node, node_vec);

struct Graph {
  struct node_vec nodes;
  struct edge_vec edges;
};

// populates references inside a graph
void graph_construct(struct Graph *graph);
void graph_destroy(struct Graph *graph);
void graph_copy(const struct Graph *graph, struct Graph *output);

//dumps graph details to stdio
void graph_dump(struct Graph *graph);

// finds a node having a specific id
struct node *graph_find_node(struct Graph *graph, int id);

struct node *graph_get_neighbour_of(struct node *node, struct edge *edge);
#endif
