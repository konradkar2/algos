#include "../utils/da_append.h"
#include <ctype.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct node;
vector_of(struct node *, ref_node_vec);

struct edge {
  int ids[2];
  // populated later
  struct ref_node_vec nodes;
};

vector_of(struct edge, edge_vec);
vector_of(struct edge *, ref_edge_vec);

struct node {
  int id;
  // populated later
  bool drawn;
  struct ref_edge_vec edges;
};

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

  // da_append_safe(&graph.nodes, (struct node){.id = 105});
  // da_append_safe(&graph.nodes, (struct node){.id = 106});

  da_append_safe(&graph.edges, ((struct edge){.ids = {100, 101}}));
  // da_append_safe(&graph.edges, ((struct edge){.ids = {105, 106}}));

  return graph;
}

struct node *find_node(const struct node_vec *nodes, int id) {
  for (size_t i = 0; i < nodes->count; ++i) {
    struct node *current_node = &nodes->items[i];
    if (current_node->id == id) {
      return current_node;
    }
  }
  return NULL;
}

// populates references inside graph
void prepare_graph(struct Graph *graph) {
  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *current_edge = &graph->edges.items[i];
    for (int j = 0; j < 2; ++j) {
      struct node *node = find_node(&graph->nodes, current_edge->ids[j]);
      da_append_safe(&node->edges, current_edge);
      da_append_safe(&current_edge->nodes, node);
    }
  }
}

#define NODERADIUS 30
#define NODEHITBOXRADIUS (2 * NODERADIUS)
#define EDGETHICKNESS 2
#define NODEDISTANCE (NODERADIUS * 4)
#define CLOCKSPLIT 3

Vector2 find_pos_for_node(Vector2 reference_pos, const struct node *node,
                          struct Graph *graph) {
  for (int i = 0; i < CLOCKSPLIT; ++i) {
    int xprim = reference_pos.x + cos(2.0 * PI / (i + 1.0)) * NODEDISTANCE;
    int yprim = reference_pos.y + sin(2.0 * PI / (i + 1.0)) * NODEDISTANCE;
    DrawCircle(xprim, yprim, NODERADIUS, BLUE);
  }
  (void)node;
  (void)graph;

  return (Vector2){0, 0};
}

void draw_node(const struct node *node, Vector2 pos) {
  DrawCircle(pos.x, pos.y, NODERADIUS, RED);
  Vector2 vec =
      MeasureTextEx(GetFontDefault(), TextFormat("%d", node->id), 25, 2);
  DrawText(TextFormat("%d", node->id), pos.x - vec.x / 2, pos.y - vec.y / 2, 25,
           BLUE);
}

int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");
  SetRandomSeed(1050);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // DrawText("Congrats! You created your first window!", 190, 200, 20,
    //          LIGHTGRAY);
    struct Graph graph = getGraph();
    prepare_graph(&graph);

    for (size_t i = 0; i < graph.edges.count; ++i) {
      struct edge *current_edge = &graph.edges.items[i];
      for (int j = 0; j < 1; j++) {
        struct node *node = find_node(&graph.nodes, current_edge->ids[j]);
        if (!node->drawn) {
          const Vector2 ref_pos = {200 * (i + j + 1), 200 * (i + j + 1)};
          draw_node(node, ref_pos);
          find_pos_for_node(ref_pos, node, &graph);
          node->drawn = true;
        }
      }
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
