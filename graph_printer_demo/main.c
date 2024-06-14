#include "../common/graph.h"
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

  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {100, 101}, .id = 200}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {101, 102}, .id = 201}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {102, 103}, .id = 202}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {103, 104}, .id = 204}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {105, 101}, .id = 205}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {105, 106}, .id = 206}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {101, 107}, .id = 207}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {103, 108}, .id = 208}));
  da_append_safe(&graph.edges,
                 ((struct edge){.node_ids = {102, 107}, .id = 209}));
  // da_append_safe(&graph.edges,
  //                ((struct edge){.node_ids = {106, 107}, .id = 210}));
  // da_append_safe(&graph.edges,
  //                ((struct edge){.node_ids = {108, 101}, .id = 211}));
  //da_append_safe(&graph.edges,
  //               ((struct edge){.node_ids = {107, 103}, .id = 212}));

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
