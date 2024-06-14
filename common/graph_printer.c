#include "graph.h"
#include "graph_algos.h"
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

  FOR_EACH_NODE_IN_GRAPH(graph, node_it) {
    if (!node_it->visited || node_it->id == nodeA->id ||
        node_it->id == nodeB->id) {
      continue;
    }

    if (CheckCollisionCircleLine(node_it->pos, NODEHITBOXRADIUS, posA, posB)) {
      return true;
    }
  }

  return false;
}

bool is_edge_colliding_with_any_edges(struct Graph *graph,
                                      const struct edge *edge, Vector2 posA,
                                      Vector2 posB) {
  FOR_EACH_EDGE_IN_GRAPH(graph, edge_it) {
    if (!edge_it->visited || edge_it->id == edge->id) {
      continue;
    }

    if (CheckCollisionLines(posA, posB, edge_it->nodes[0]->pos,
                            edge_it->nodes[1]->pos, NULL)) {
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
  FOR_EACH_NODE_IN_GRAPH(graph, node_it) {
    if (!node_it->visited) {
      continue;
    }
    if (node_it->id == node->id) {
      continue;
    }
    if (node_it->id == drawn_node->id) {
      continue;
    }

    if (CheckCollisionCircles(node_pos, NODEHITBOXRADIUS, node_it->pos,
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

bool calculate_nodes_neigbour(struct Graph *graph, struct node *calculated_node,
                              struct node *neighbour, struct edge *edge) {
  printf("calculating neighbour, id: %d\n", neighbour->id);
  assert(calculated_node->id != neighbour->id);

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
  return true;
}

void reset_graph(struct Graph *graph) {
  FOR_EACH_NODE_IN_GRAPH(graph, node_it) {
    node_it->visited = false;
    node_it->pos = (Vector2){0};
  }

  FOR_EACH_EDGE_IN_GRAPH(graph, edge_it) { edge_it->visited = false; }
}

struct drawing_state {
  struct Graph *graph;
  bool drawing_failed;
};

bool on_visit_callback(void *udata, struct node *from, struct node *to,
                       struct edge *edge) {

  struct drawing_state *state = udata;
  if (from) {
    if (!calculate_nodes_neigbour(state->graph, from, to, edge)) {
      state->drawing_failed = true;
      return false;
    }
  }
  return true;
}

bool calculate_unvisited_edges(struct Graph *graph) {
  FOR_EACH_EDGE_IN_GRAPH(graph, edge_it) {
    if (edge_it->visited == false) {
      if (false == can_draw_edge_without_collision(graph, edge_it)) {
        printf("can't draw edge %d\n", edge_it->id);
        return false;
      }
    }
  }
  return true;
}

bool calculate_nodes_pos_in_graph(struct Graph *graph, Vector2 root_node_pos) {
  FOR_EACH_NODE_IN_GRAPH(graph, root_node_it) {
    reset_graph(graph);

    struct drawing_state state = {.graph = graph, .drawing_failed = false};

    printf("\nStarting to draw with root node id %d\n", root_node_it->id);
    root_node_it->pos = root_node_pos;

    dfs_visit_node(NULL, root_node_it, NULL, &state, on_visit_callback);

    if (state.drawing_failed == true ||
        false == calculate_unvisited_edges(graph)) {
      printf("drawing failed\n");
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
  FOR_EACH_EDGE_IN_GRAPH(graph, edge_it) { draw_edge(edge_it); }
  FOR_EACH_NODE_IN_GRAPH(graph, node_it) { draw_node(node_it); }
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
  graph_dump(&graph_tmp);
  draw_graph(&graph_tmp);

  return true;
}
