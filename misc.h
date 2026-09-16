// # define MISC_VECTOR // testing
// # define MISC_VECTOR_IMPLEMENTATION // testing

# ifndef MISC_LIBRARY_IMPORT
# define MISC_LIBRARY_IMPORT

# ifdef MISC_VECTOR
# include <stddef.h>
#include <stdlib.h>

/// length, capacity, a, b, ...
///                   ^ pointer

typedef struct {
    size_t count;
    size_t capacity;
} MISC_Vector_Header; // Datatype to prefix the vector

/// Simple way to create stack based vector.
/// Usage: `int* variable = vec_new(int, 1, 2, 3);`
# define vec_new(type, ...) (type*)\
((MISC_Vector_Header *)(&(struct {                                               \
    size_t count;                                                                \
    size_t capacity;                                                             \
    type data[sizeof((type[]) {__VA_ARGS__}) / sizeof(type)];                    \
}) {                                                                             \
    .count = sizeof((type[]) {__VA_ARGS__}) / sizeof(type),                      \
    .capacity = sizeof((type[]) {__VA_ARGS__}) / sizeof(type),                   \
    .data = {__VA_ARGS__}                                                        \
}) + 1)

# define vec_count(vector) (((MISC_Vector_Header*)(vector) - 1) -> count)

/// Allocates the vector with specified size
/// NOTICE: vectors allocated by it MUST be destroyed using vec_destroy
static inline void* vec_alloc(size_t sizeof_value, size_t capacity) {
    MISC_Vector_Header* header = malloc(sizeof(MISC_Vector_Header) + sizeof_value * capacity);
    if (header == NULL) return NULL;
    header -> count = 0;
    header -> capacity = capacity;
    return header + 1;
}

/// All the vectors made in the way besides vec_new must be destroyed,
/// but due to their internal complexity regular free() won't work
# define vec_destroy(vector) (vector != NULL) ? free((MISC_Vector_Header*)(vector) - 1) : NULL

# endif

# ifdef MISC_VECTOR_IMPLEMENTATION
# undef MISC_VECTOR_IMPLEMENTATION

# endif

# endif

// int main(void) {
// #include <stdio.h>
//     int* test = vec_new(int, 1 + 2, 2, 5);
//     printf("%lu\n", vec_count(test));
//     vec_count(test) = 1;
//     printf("%lu\n", vec_count(test));
//
//     int* test2 = vec_alloc(sizeof(int), 5);
//     printf("%lu\n", vec_count(test2));
//     vec_destroy(test2);
// }
