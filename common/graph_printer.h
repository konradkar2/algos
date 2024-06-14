#ifndef GRAPH_PRINTER_H
#define GRAPH_PRINTER_H

#include "graph.h"
#include <raylib.h>

// draws a graph in a box defined by pos, width and height
// if algorithm is unable to draw the graph returns false
bool graph_printer_draw(const struct Graph *graph, Vector2 pos, int width,
                        int height);

#endif
