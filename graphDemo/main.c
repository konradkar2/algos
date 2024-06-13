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
  bool visited;
  int id;
};

vector_of(struct edge, edge_vec);
vector_of(struct edge *, ref_edge_vec);

struct node {
  int id;
  // populated later
  bool visited;
  struct ref_edge_vec edges;
  Vector2 pos;
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
  da_append_safe(&graph.nodes, (struct node){.id = 105});
  da_append_safe(&graph.nodes, (struct node){.id = 106});
  da_append_safe(&graph.nodes, (struct node){.id = 107});
  da_append_safe(&graph.nodes, (struct node){.id = 108});

  // da_append_safe(&graph.nodes, (struct node){.id = 105});
  // da_append_safe(&graph.nodes, (struct node){.id = 106});

  da_append_safe(&graph.edges, ((struct edge){.ids = {100, 101}, .id = 200}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {101, 102}, .id = 201}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {102, 103}, .id = 202}));
  // da_append_safe(&graph.edges, ((struct edge){.ids = {101, 104}, .id =
  // 203}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {103, 104}, .id = 204}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {105, 101}, .id = 205}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {105, 106}, .id = 206}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {101, 107}, .id = 207}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {103, 108}, .id = 208}));
  da_append_safe(&graph.edges, ((struct edge){.ids = {102, 107}, .id = 209}));
  //da_append_safe(&graph.edges, ((struct edge){.ids = {107, 104}, .id = 210}));
  //  da_append_safe(&graph.edges, ((struct edge){.ids = {105, 106}}));

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

void dump_graph(struct Graph *graph) {
  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *edge = &graph->edges.items[i];
    struct node **nodes = edge->nodes.items;
    printf("edge (id: %d, drawn: %d), nodes: (%d drawn: %d, %d, drawn: %d)\n",
           edge->id, edge->visited, nodes[0]->id, nodes[0]->visited,
           nodes[1]->id, nodes[1]->visited);
  }
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
#define CLOCKSPLIT 12

bool is_edge_colliding_with_any_nodes(struct Graph *graph,
                                      const struct edge *edge, Vector2 posA,
                                      Vector2 posB) {
  const struct node *nodeA = edge->nodes.items[0];
  const struct node *nodeB = edge->nodes.items[1];

  for (size_t k = 0; k < graph->nodes.count; ++k) {
    const struct node *node = &graph->nodes.items[k];
    if (!node->visited || node->id == nodeA->id || node->id == nodeB->id) {
      continue;
    }

    if (CheckCollisionCircleLine(node->pos, NODEHITBOXRADIUS, posA, posB)) {
      return true;
    }
  }

  return false;
}

bool is_edge_colliding_with_any_edges(struct Graph *graph,
                                      const struct edge *edge, Vector2 posA,
                                      Vector2 posB) {
  for (size_t j = 0; j < graph->edges.count; ++j) {
    const struct edge *curr_edge = &graph->edges.items[j];
    if (!curr_edge->visited || curr_edge->id == edge->id) {
      continue;
    }

    if (CheckCollisionLines(posA, posB, curr_edge->nodes.items[0]->pos,
                            curr_edge->nodes.items[1]->pos, NULL)) {
      return true;
    }
  }

  return false;
}

bool can_draw_node_and_edge_without_collision(struct Graph *graph,
                                              const struct node *drawn_node,
                                              const struct node *node,
                                              Vector2 node_pos,
                                              const struct edge *edge) {

  assert(drawn_node->visited == true);

  // check if edge will collide with any of edges
  if (is_edge_colliding_with_any_edges(graph, edge, drawn_node->pos,
                                       node_pos)) {
    return false;
  }

  // check if edge will collide with any of nodes
  if (is_edge_colliding_with_any_nodes(graph, edge, drawn_node->pos,
                                       node_pos)) {
    return false;
  }

  // check if node will collide with any of node
  for (size_t k = 0; k < graph->nodes.count; ++k) {
    const struct node *curr_node = &graph->nodes.items[k];

    if (!curr_node->visited) {
      continue;
    }
    if (curr_node->id == node->id) {
      continue;
    }
    if (curr_node->id == drawn_node->id) {
      continue;
    }

    if (CheckCollisionCircles(node_pos, NODEHITBOXRADIUS, curr_node->pos,
                              NODEHITBOXRADIUS)) {
      return false;
    }
  }
  return true;
}

// nodeA, nodeB are already drawn
bool can_draw_edge_without_collision(struct Graph *graph,
                                     const struct edge *edge) {
  const struct node *nodeA = edge->nodes.items[0];
  const struct node *nodeB = edge->nodes.items[1];
  assert(edge->visited == false);
  assert(nodeA->visited == true);
  assert(nodeB->visited == true);

  if (is_edge_colliding_with_any_edges(graph, edge, nodeA->pos, nodeB->pos)) {
    return false;
  }
  if (is_edge_colliding_with_any_nodes(graph, edge, nodeA->pos, nodeB->pos)) {
    return false;
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
  assert(drawn_node->visited == true);
  assert(neighbour->visited == false);

  const double step = 2.0 * PI / CLOCKSPLIT;
  for (int i = 0; i < CLOCKSPLIT; ++i) {
    Vector2 possible_pos = {0};

    possible_pos.x = drawn_node->pos.x + cos(step * i) * R;
    possible_pos.y = drawn_node->pos.y + sin(step * i) * R;
    if (true == can_draw_node_and_edge_without_collision(
                    graph, drawn_node, neighbour, possible_pos, edge)) {
      *result_pos = possible_pos;
      return true;
    }
  }
  return false;
}

bool calculate_nodes_neighbours(struct Graph *graph, struct node *drawn_node);

bool calculate_nodes_neigbour(struct Graph *graph, struct node *calculated_node,
                              struct node *neighbour, struct edge *edge) {
  printf("calculating neighbour, id: %d\n", neighbour->id);
  assert(calculated_node->id != neighbour->id);
  assert(calculated_node->visited == true);

  if (neighbour->visited && edge->visited == true) {
    return true;
  }

  if (neighbour->visited && edge->visited == false) {
    printf("nodes calculate, calculating edge...\n");
    if (can_draw_edge_without_collision(graph, edge)) {
      edge->visited = true;
      return true;
    }
    return false;
  }

  Vector2 pos_for_neighbour = {0};
  bool pos_found = false;
  for (size_t radius = NODEDISTANCE; radius < NODEDISTANCE * 3; radius += 10) {
    pos_found = try_get_pos_for_neigbour(graph, calculated_node, neighbour,
                                         edge, radius, &pos_for_neighbour);
    if (pos_found == true) {
      break;
    }
  }

  if (!pos_found) {
    return false;
  }

  printf("node (id %d), neighbour of (id %d), calculated to be at x=%f, y=%f\n",
         neighbour->id, calculated_node->id, pos_for_neighbour.x,
         pos_for_neighbour.y);

  neighbour->pos = pos_for_neighbour;
  neighbour->visited = true;
  edge->visited = true;

  if (!calculate_nodes_neighbours(graph, neighbour)) {
    // we failed to draw neigbour's neigbours so lets cancel drawing
    // process
    return false;
  }

  return true;
}

struct node *get_neighbour(struct node *node, struct edge *edge) {
  assert(edge->nodes.count == 2);
  if (edge->nodes.items[0]->id == node->id) {
    return edge->nodes.items[1];
  } else {
    return edge->nodes.items[0];
  }
}

bool calculate_nodes_neighbours(struct Graph *graph, struct node *drawn_node) {
  printf("drawing neighbours for node %d\n", drawn_node->id);

  assert(drawn_node->visited == true);

  for (size_t i = 0; i < drawn_node->edges.count; ++i) {
    struct edge *edge = drawn_node->edges.items[i];
    if (edge->visited) {
      continue;
    }

    struct node *neighbour = get_neighbour(drawn_node, edge);
    if (!calculate_nodes_neigbour(graph, drawn_node, neighbour, edge)) {
      return false;
    }
  }
  return true;
}

void reset_graph(struct Graph *graph) {
  for (size_t i = 0; i < graph->nodes.count; ++i) {
    struct node *node = &graph->nodes.items[i];
    node->visited = false;
    node->pos = (Vector2){0};
  }

  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *edge = &graph->edges.items[i];
    edge->visited = false;
  }
}

bool calculate_graph(struct Graph *graph, Vector2 startPos) {
  for (size_t i = 0; i < graph->nodes.count; ++i) {
    reset_graph(graph);
    struct node *root_node = &graph->nodes.items[i];
    printf("\nStarting to draw with root node id %d\n", root_node->id);
    root_node->visited = true;
    root_node->pos = startPos;
    if (false == calculate_nodes_neighbours(graph, root_node)) {
      continue;
    }
    return true;
  }
  return false;
}

void draw_node(const struct node *node) {

  DrawCircle(node->pos.x, node->pos.y, NODERADIUS, RED);
  Vector2 vec =
      MeasureTextEx(GetFontDefault(), TextFormat("%d", node->id), 25, 2);
  DrawText(TextFormat("%d", node->id), node->pos.x - vec.x / 2,
           node->pos.y - vec.y / 2, 25, BLUE);
}

void draw_edge(const struct edge *edge) {
  const struct node *nodeA = edge->nodes.items[0];
  const struct node *nodeB = edge->nodes.items[1];
  DrawLineV(nodeA->pos, nodeB->pos, BLACK);
}

void draw_graph(const struct Graph *graph) {
  for (size_t i = 0; i < graph->edges.count; ++i) {
    struct edge *edge = &graph->edges.items[i];
    draw_edge(edge);
  }

  for (size_t i = 0; i < graph->nodes.count; ++i) {
    struct node *node = &graph->nodes.items[i];
    draw_node(node);
  }
}
int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");
  SetRandomSeed(1050);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    struct Graph graph = getGraph();
    dump_graph(&graph);
    if (!calculate_graph(&graph, (Vector2){.x = 300, .y = 200})) {
      printf("failed to draw_graph\n");
      return 0;
    } else {
      draw_graph(&graph);
      printf("graph drawn!\n");
      dump_graph(&graph);
    }

    EndDrawing();
    // pause();
  }

  CloseWindow();

  return 0;
}
