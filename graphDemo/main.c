#include "../utils/da_append.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct node;
vector_of(struct node *, ref_node_vec);

struct edge {
  int ids[2];
  // populated later
  struct ref_node_vec nodes;
  bool drawn;
  int hash;
};

vector_of(struct edge, edge_vec);
vector_of(struct edge *, ref_edge_vec);

struct node {
  int id;
  // populated later
  bool drawn;
  struct ref_edge_vec edges;
  Vector2 pos;
  int hash;
};

vector_of(struct node, node_vec);

struct Graph {
  struct node_vec nodes;
  struct edge_vec edges;
};

void prepare_graph(struct Graph *graph);
struct Graph getGraph(void) {
  struct Graph graph = {0};

  // struct node node1 = {.id = 100};
  da_append_safe(&graph.nodes, (struct node){.id = 100});
  da_append_safe(&graph.nodes, (struct node){.id = 101});
  da_append_safe(&graph.nodes, (struct node){.id = 102});
  da_append_safe(&graph.nodes, (struct node){.id = 103});
  da_append_safe(&graph.nodes, (struct node){.id = 104});

  // da_append_safe(&graph.nodes, (struct node){.id = 105});
  // da_append_safe(&graph.nodes, (struct node){.id = 106});

  da_append_safe(&graph.edges, ((struct edge){.ids = {100, 101}}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {101, 102}}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {101, 103}}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {101, 104}}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {103, 102}}));
  // da_append_safe(&graph.edges, ((struct edge){.ids = {105, 106}}));

  prepare_graph(&graph);

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

static int object_hash = 0;
// populates references inside graph
void prepare_graph(struct Graph *graph) {
  // just iterate an integer for now
  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *current_edge = &graph->edges.items[i];
    current_edge->hash = object_hash++;
    for (int j = 0; j < 2; ++j) {
      struct node *node = find_node(&graph->nodes, current_edge->ids[j]);
      da_append_safe(&node->edges, current_edge);
      da_append_safe(&current_edge->nodes, node);
    }
  }
  object_hash += 100;

  for (size_t i = 0; i < graph->nodes.count; ++i) {
    struct node *current_node = &graph->nodes.items[i];
    current_node->hash = object_hash++;
  }
}

#define NODERADIUS 30
#define NODEHITBOXRADIUS (2 * NODERADIUS)
#define EDGETHICKNESS 2
#define NODEDISTANCE (NODERADIUS * 4)
#define CLOCKSPLIT 12

// nodeA is already drawn node
bool can_draw_at_without_collision(struct Graph *graph,
                                   const struct node *drawn_node,
                                   const struct node *node, Vector2 node_pos,
                                   const struct edge *edge) {
  // check if edge will collide with any of edges

  assert(drawn_node->drawn == true);

  for (size_t j = 0; j < graph->edges.count; ++j) {
    const struct edge *curr_edge = &graph->edges.items[j];
    if (!curr_edge->drawn || curr_edge->hash == edge->hash) {
      continue;
    }

    if (CheckCollisionLines(drawn_node->pos, node_pos,
                            curr_edge->nodes.items[0]->pos,
                            curr_edge->nodes.items[1]->pos, NULL)) {
      return false;
    }
  }
  // check if edge will collide with any of nodes
  for (size_t k = 0; k < graph->nodes.count; ++k) {
    const struct node *curr_node = &graph->nodes.items[k];
    if (!curr_node->drawn || curr_node->hash == drawn_node->hash ||
        curr_node->hash == node->hash) {
      continue;
    }
    if (CheckCollisionCircleLine(curr_node->pos, NODEHITBOXRADIUS,
                                 drawn_node->pos, node_pos)) {
      return false;
    }
  }

  // check if node will collide with any of node
  for (size_t k = 0; k < graph->nodes.count; ++k) {
    const struct node *curr_node = &graph->nodes.items[k];

    if (!curr_node->drawn) {
      continue;
    }
    if (curr_node->hash == node->hash) {
      continue;
    }
    if (curr_node->hash == drawn_node->hash) {
      continue;
    }

    if (CheckCollisionCircles(node_pos, NODEHITBOXRADIUS, curr_node->pos,
                              NODEHITBOXRADIUS)) {
      return false;
    }
  }
  return true;
}

// try to find a position for the nodeB/edge to be drawn on circle of radius R
// around nodeA
bool try_get_pos_for_neigbour(struct Graph *graph,
                              const struct node *drawn_node,
                              const struct node *neighbour,
                              const struct edge *edge, double R,
                              Vector2 *result_pos) {
  assert(drawn_node->drawn == true);
  assert(neighbour->drawn == false);

  const double step = 2.0 * PI / CLOCKSPLIT;
  for (int i = 0; i < CLOCKSPLIT; ++i) {
    Vector2 possible_pos = {0};
    possible_pos.x = drawn_node->pos.x + cos(step * i) * R;
    possible_pos.y = drawn_node->pos.y + sin(step * i) * R;
    if (true == can_draw_at_without_collision(graph, drawn_node, neighbour,
                                              possible_pos, edge)) {
      *result_pos = possible_pos;
      return true;
    }
  }
  return false;
}

void draw_node_at(const struct node *node, Vector2 pos) {
  printf("drawing node (id %d) at x: %f, y: %f\n", node->id, pos.x, pos.y);

  DrawCircle(pos.x, pos.y, NODERADIUS, RED);
  Vector2 vec =
      MeasureTextEx(GetFontDefault(), TextFormat("%d", node->id), 25, 2);
  DrawText(TextFormat("%d", node->id), pos.x - vec.x / 2, pos.y - vec.y / 2, 25,
           BLUE);
}

void draw_edge_at(const struct edge *edge, Vector2 posA, Vector2 posB) {
  (void)edge;
  DrawLineV(posA, posB, BLACK);
}

bool draw_nodes_neighbours(struct Graph *graph, struct node *drawn_node);

bool draw_nodes_neigbour(struct Graph *graph, struct node *drawn_node,
                         struct node *neighbour, struct edge *edge) {
  printf("drawing neighbour, id: %d, hash: %d\n", neighbour->id,
         neighbour->hash);
  assert(drawn_node->hash != neighbour->hash);
  assert(drawn_node->drawn == true);

  if (neighbour->drawn) {
    return true;
  }

  for (size_t radius = NODEDISTANCE; radius < NODEDISTANCE * 3; radius += 10) {
    Vector2 pos_for_neighbour;
    if (try_get_pos_for_neigbour(graph, drawn_node, neighbour, edge, radius,
                                 &pos_for_neighbour)) {
      draw_node_at(neighbour, pos_for_neighbour);
      neighbour->pos = pos_for_neighbour;
      neighbour->drawn = true;
      draw_edge_at(edge, drawn_node->pos, pos_for_neighbour);
      edge->drawn = true;

      if (!draw_nodes_neighbours(graph, neighbour)) {
        // we failed to draw neigbour's neigbours so lets cancel drawing
        // process
        return false;
      }
      break;
    }
  }

  return neighbour->drawn;
}

struct node *get_neighbour(struct node *node, struct edge *edge) {
  assert(edge->nodes.count == 2);
  if (edge->nodes.items[0]->hash == node->hash) {
    return edge->nodes.items[1];
  } else {
    return edge->nodes.items[0];
  }
}

bool draw_nodes_neighbours(struct Graph *graph, struct node *drawn_node) {
  printf("drawing neighbours for node %d hash %d\n", drawn_node->id,
         drawn_node->hash);

  assert(drawn_node->drawn == true);

  for (size_t i = 0; i < drawn_node->edges.count; ++i) {
    struct edge *edge = drawn_node->edges.items[i];
    if (edge->drawn) {
      continue;
    }

    struct node *neighbour = get_neighbour(drawn_node, edge);
    if (!draw_nodes_neigbour(graph, drawn_node, neighbour, edge)) {
      return false;
    }
  }
  return true;
}

bool draw_graph(struct Graph *graph, Vector2 startPos) {
  for (size_t i = 0; i < graph->nodes.count; ++i) {
    struct node *root_node = &graph->nodes.items[i];
    root_node->drawn = true;
    root_node->pos = startPos;
    if (false == draw_nodes_neighbours(graph, root_node)) {
      root_node->drawn = false;
      root_node->pos = (Vector2){0};
      continue;
    }
    draw_node_at(root_node, startPos);
    return true;
  }
  return false;
}

int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");
  SetRandomSeed(1050);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    struct Graph graph = getGraph();

    if (!draw_graph(&graph, (Vector2){.x = 300, .y = 200})) {
      printf("failed to draw_graph\n");
      return 0;
    } else {
      printf("graph drawn!\n");
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
