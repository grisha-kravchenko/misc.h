// # define MISC_VECTOR // testing
// # define MISC_VECTOR_IMPLEMENTATION // testing

# ifndef MISC_LIBRARY_IMPORT
# define MISC_LIBRARY_IMPORT

# ifdef MISC_VECTOR
# include <stddef.h>

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

# endif

# ifdef MISC_VECTOR_IMPLEMENTATION
# undef MISC_VECTOR_IMPLEMENTATION

# endif

# endif

// int main(void) {
//     int* test = vec_new(int, 1 + 2, 2);
// }
