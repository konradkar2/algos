#include "../common/graph.h"
#include "../common/graph_algos.h"
#include "../common/graph_printer.h"
#include "../utils/da_append.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct Graph getGraph(void) {
  struct Graph graph = {0};

  graph_add_node(&graph, 100);
  graph_add_node(&graph, 101);
  graph_add_node(&graph, 102);
  graph_add_node(&graph, 103);
  graph_add_node(&graph, 104);
  graph_add_node(&graph, 105);
  graph_add_node(&graph, 106);
  graph_add_node(&graph, 107);
  graph_add_node(&graph, 108);

  graph_add_edge(&graph, 100, 101);
  graph_add_edge(&graph, 101, 102);
  graph_add_edge(&graph, 102, 103);
  graph_add_edge(&graph, 103, 104);
  graph_add_edge(&graph, 105, 101);
  graph_add_edge(&graph, 105, 106);
  graph_add_edge(&graph, 101, 107);
  graph_add_edge(&graph, 103, 108);
  graph_add_edge(&graph, 102, 107);
  graph_add_edge(&graph, 108, 101);

  graph_construct(&graph);

  return graph;
}

int main(void) {
  int w = 1400;
  int h = 800;

  InitWindow(w, h, "raylib [core] example - basic window");
  SetRandomSeed(1050);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    struct Graph graph = getGraph();
    
    struct graph_vec cycles = find_cycles(&graph, &graph.nodes.items[0]);
    printf("found %zuu cycles!\n", cycles.count);

    
    if (!graph_printer_draw(&graph, (Vector2){0}, w, h)) {
      printf("failed to draw graph\n");
      return 1;
    }

    EndDrawing();
    // pause();
  }

  CloseWindow();

  return 0;
}
