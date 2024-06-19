#ifndef DA_ARRAY_H
#define DA_ARRAY_H

#include <assert.h>
#include <stdlib.h>

#define vector_of(type, name)                                                  \
  struct name {                                                                \
    type *items;                                                               \
    size_t count;                                                              \
    size_t capacity;                                                           \
  }

#define da_append_safe(xs, x)                                                  \
  do {                                                                         \
    if ((xs)->count >= (xs)->capacity) {                                       \
      if ((xs)->capacity == 0)                                                 \
        (xs)->capacity = 64;                                                   \
      else                                                                     \
        (xs)->capacity *= 2;                                                   \
      (xs)->items =                                                            \
          realloc((xs)->items, (xs)->capacity * sizeof(*(xs)->items));         \
      assert((xs)->items != NULL);                                             \
    }                                                                          \
    (xs)->items[(xs)->count++] = (x);                                          \
  } while (0)

#define da_reset(xs)                                                           \
  do {                                                                         \
    free((xs)->items);                                                         \
    (xs)->items = NULL;                                                        \
    (xs)->count = 0;                                                           \
    (xs)->capacity = 0;                                                        \
  } while (0)

#define FOR_EACH_EDGE_IN_GRAPH(GRAPH, EDGE_IT)                                 \
  for (struct edge *EDGE_IT = &GRAPH->edges.items[0];                          \
       EDGE_IT < &GRAPH->edges.items[GRAPH->edges.count]; ++EDGE_IT)

#define DA_FOR_EACH(xs, IDX) for (size_t IDX = 0; IDX < (xs)->count; ++IDX)


#endif
