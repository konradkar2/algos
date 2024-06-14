#ifndef GRAPH_ALGOS_H
#define GRAPH_ALGOS_H

#include "graph.h"

uint get_graph_smallest_deg(struct Graph *graph);
uint get_node_deg(const struct node *node);

typedef bool (*dfs_callback)(void *udata, struct node *from, struct node *to,
                             struct edge *edge);

void depth_first_search(struct Graph *graph, void *udata, dfs_callback cb);
void dfs_visit_node(struct node *already_visited_node, struct node *node,
                    struct edge *edge, void *udata, dfs_callback callback);

#endif
