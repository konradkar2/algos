#include "graph_algos.h"

unsigned smallest_deg(struct Graph * graph)
{
    (void)graph;
    return 0;
}

unsigned deg(const struct node * node)
{
    return node->edges.count;
}
