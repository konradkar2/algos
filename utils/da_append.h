#ifndef DA_ARRAY_H
#define DA_ARRAY_H

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

#endif
