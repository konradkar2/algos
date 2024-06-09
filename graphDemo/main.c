#include "../utils/da_append.h"
#include <raylib.h>
#include <stdlib.h>

struct node {
  int id;
  bool drawn;
};

struct edge {
  int aId;
  int bId;
};
vector_of(struct edge, edge_vec);
vector_of(struct node, node_vec);

struct Graph {
  struct node_vec nodes;
  struct edge_vec edges;
};

struct Graph getGraph(void) {
  struct Graph graph = {0};

  // struct node node1 = {.id = 100};
  da_append_safe(&graph.nodes, (struct node){.id = 100});
  da_append_safe(&graph.nodes, (struct node){.id = 101});

  da_append_safe(&graph.edges, ((struct edge){.aId = 100, .bId = 101}));

  return graph;
}

#define NODERADIUS 20
#define EDGETHICKNESS 2
#define NODEDISTANCE (NODERADIUS * 4)

struct node *find_node(const struct node_vec *nodes, int id) {
  for (size_t i = 0; i < nodes->count; ++i) {
    struct node *current_node = &nodes->items[i];
    if (current_node->id == id) {
      return current_node;
    }
  }
  return NULL;
}

int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);
    struct Graph graph = getGraph();
    for (size_t i = 0; i < graph.edges.count; ++i) {
      struct edge *current_edge = &graph.edges.items[i];
      struct node *node_a = find_node(&graph.nodes, current_edge->aId);
      struct node *node_b = find_node(&graph.nodes, current_edge->bId);
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
