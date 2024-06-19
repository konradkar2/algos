#ifndef GRAPH_ALGOS_H
#define GRAPH_ALGOS_H

#include "graph.h"

uint get_graph_smallest_deg(struct Graph *graph);
uint get_node_deg(const struct node *node);

typedef bool (*dfs_callback)(void *udata, struct node *from, struct node *to,
                             struct edge *edge);

void depth_first_search(struct Graph *graph, void *udata, dfs_callback cb);

// every node will be visited in a connected graph, but not every edge if graph
// has cycles
void dfs_visit_node(struct node *node_from, struct node *node_to,
                    struct edge *edge, void *udata, dfs_callback callback);

// modified version of dfs_visit_node for traversing all edges (e.g. for cycle
// detection)
void dfs_edge_visit_node(struct node *node_from, struct node *node_to,
                         struct edge *edge, void *udata, dfs_callback callback);

vector_of(struct Graph, graph_vec);
struct graph_vec find_cycles(const struct Graph *graph, struct node *root_node);

#endif
