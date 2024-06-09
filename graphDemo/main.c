#include "../utils/da_append.h"
#include <raylib.h>
#include <stdlib.h>

struct node {
  int id;
};

struct edge {
  struct node *a;
  struct node *b;
};
vector_of(struct edge, edge_vec);
vector_of(struct node, node_vec);

struct Graph {
  struct node_vec nodes;
  struct edge_vec edges;
};

int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
