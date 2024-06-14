#include "graph.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define NODERADIUS 30
#define NODEHITBOXRADIUS (2 * NODERADIUS)
#define EDGETHICKNESS 2
#define NODEDISTANCE (NODERADIUS * 4)
#define CLOCKSPLIT 12

bool is_edge_colliding_with_any_nodes(struct Graph *graph,
                                      const struct edge *edge, Vector2 posA,
                                      Vector2 posB) {
  const struct node *nodeA = edge->nodes[0];
  const struct node *nodeB = edge->nodes[1];

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

    if (CheckCollisionLines(posA, posB, curr_edge->nodes[0]->pos,
                            curr_edge->nodes[1]->pos, NULL)) {
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
  const struct node *nodeA = edge->nodes[0];
  const struct node *nodeB = edge->nodes[1];
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

bool calculate_nodes_neighbours(struct Graph *graph, struct node *drawn_node) {
  printf("drawing neighbours for node %d\n", drawn_node->id);

  assert(drawn_node->visited == true);

  for (size_t i = 0; i < drawn_node->edges.count; ++i) {
    struct edge *edge = drawn_node->edges.items[i];
    if (edge->visited) {
      continue;
    }

    struct node *neighbour = graph_get_neighbour_of(drawn_node, edge);
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

bool calculate_nodes_pos_in_graph(struct Graph *graph, Vector2 root_node_pos) {
  for (size_t i = 0; i < graph->nodes.count; ++i) {
    reset_graph(graph);
    struct node *root_node = &graph->nodes.items[i];
    printf("\nStarting to draw with root node id %d\n", root_node->id);
    root_node->visited = true;
    root_node->pos = root_node_pos;
    if (false == calculate_nodes_neighbours(graph, root_node)) {
      graph_dump(graph);
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
  const struct node *nodeA = edge->nodes[0];
  const struct node *nodeB = edge->nodes[1];
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

bool graph_printer_draw(const struct Graph *graph, Vector2 pos, int width,
                        int height) {
  struct Graph graph_tmp = {0};
  graph_copy(graph, &graph_tmp);

  Vector2 root_node_pos = {.x = (pos.x + width) / 2.0,
                           .y = (pos.y + height) / 2.0};
  if (!calculate_nodes_pos_in_graph(&graph_tmp, root_node_pos)) {
    return false;
  }
  draw_graph(&graph_tmp);

  return true;
}
