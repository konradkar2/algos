#ifndef GRAPH_ALGOS_H
#define GRAPH_ALGOS_H

#include "graph.h"

uint get_graph_smallest_deg(struct Graph *graph);
uint get_node_deg(const struct node *node);

typedef void (*dfs_callback)(struct node *already_visited, struct node *visited,
                             struct edge *edge);

void depth_first_search(struct Graph *graph, dfs_callback cb);
void dfs_visit_node(struct node *already_visited_node, struct node *node,
                    struct edge *edge, dfs_callback callback);

#endif
