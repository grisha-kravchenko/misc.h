# if 0 // testing
# define MISC_VECTOR
# define MISC_IMPLEMENTATIONS
# endif

# ifndef MISC_LIBRARY_IMPORT
# define MISC_LIBRARY_IMPORT

# ifdef MISC_VECTOR

# ifndef MISC_VECTOR_INITIAL_CAPACITY
# define MISC_VECTOR_INITIAL_CAPACITY 10
# endif

# include <stddef.h>
# include <stdlib.h>
# include <string.h>
# include <assert.h>

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
# define vec_capacity(vector) (((MISC_Vector_Header*)(vector) - 1) -> capacity)

# define vec_push(vector, value) do {                                                                                   \
    if ((vector) == NULL) (vector) = vec_alloc(sizeof((vector)[0]), MISC_VECTOR_INITIAL_CAPACITY);                      \
    if (vec_capacity(vector) <= vec_count(vector))                                                                      \
        (vector) = vec_realloc(sizeof((vector)[0]), vec_count(vector) * 2 + MISC_VECTOR_INITIAL_CAPACITY, (vector));    \
    if ((vector) == NULL) assert(0 && "Couldn't allocate vector before pushing to it.");                                \
    (vector)[vec_count(vector)] = value;                                                                                \
    vec_count(vector) += 1;                                                                                             \
} while (0)

# define vec_concat_array(vector, array, length) do {                                                                            \
    if ((vector) == NULL) (vector) = vec_alloc(sizeof((vector)[0]), (length));                                                   \
    if (vec_capacity(vector) <= vec_count(vector) + (length))                                                                    \
        (vector) = vec_realloc(sizeof(vector[0]), vec_count(vector) * 2 + MISC_VECTOR_INITIAL_CAPACITY + (length), (vector));    \
    memcpy((vector) + vec_count(vector), (array), (length));                                                                     \
    vec_count(vector) += (length);                                                                                               \
} while (0)

# define vec_contact_vec(vector, other) \
    if (other != NULL) vec_concat_array((vector), (other), vec_count(other))

// vec_from_array(type, array, count) vec_alloc(sizeof(type), size_t capacity)

/// All the vectors made in the way besides vec_new must be destroyed,
/// but due to their internal complexity regular free() won't work
# define vec_destroy(vector) (vector != NULL) ? free((MISC_Vector_Header*)(vector) - 1) : NULL

void* vec_alloc(size_t sizeof_value, size_t capacity);
void* vec_realloc(size_t sizeof_value, size_t new_capacity, void* vector);
char* vec_concat_string(char** vector);

# ifdef MISC_IMPLEMENTATIONS

/// Allocates the vector with specified size
/// NOTICE: vectors allocated by it MUST be destroyed using vec_destroy
void* vec_alloc(size_t sizeof_value, size_t capacity) {
    MISC_Vector_Header* header = malloc(sizeof(MISC_Vector_Header) + sizeof_value * capacity);
    if (header == NULL) return NULL;
    header -> count = 0;
    header -> capacity = capacity;
    return header + 1;
}

/// Reallocates the vector with specified size
void* vec_realloc(size_t sizeof_value, size_t new_capacity, void* vector) {
    MISC_Vector_Header* header = (MISC_Vector_Header*)vector - 1;
    MISC_Vector_Header* new_vec_header = realloc(header, sizeof(MISC_Vector_Header) + sizeof_value * new_capacity);
    if (new_vec_header == NULL) return NULL;
    new_vec_header -> capacity = new_capacity;
    return new_vec_header + 1;
}

char* vec_concat_string(char** vector) {
    size_t buffer_length = 1; // NULL-terminator
    for (size_t i = 0; i < vec_count(vector); ++i) buffer_length += strlen(vector[i]);
    char* string = malloc(buffer_length);
    if (string == NULL) return NULL;
    string[0] = 0;
    for (size_t i = 0; i < vec_count(vector); ++i) strcat(string, vector[i]);
    return string;
}

# endif

# endif

# ifdef MISC_IMPLEMENTATIONS
# undef MISC_IMPLEMENTATIONS
# endif

# endif

# if 0
int main(void) {
#include <stdio.h>
    int* test = vec_new(int, 1 + 2, 2, 5);
    printf("%lu\n", vec_count(test));
    vec_count(test) = 1;
    printf("%lu\n", vec_count(test));

    int* test2 = vec_alloc(sizeof(int), 5);
    printf("%lu / %lu\n", vec_count(test2), vec_capacity(test2));
    vec_push(test2, 10);
    vec_push(test2, 11);
    printf("%i\n", test2[0]);
    printf("%i\n", test2[1]);
    int* test3 = vec_realloc(sizeof(int), 10, test2);

    int* test4 = NULL;
    for (int i = 0; i < 100; ++i) {
        vec_push(test4, i);
    }
    printf("4: %lu/%lu; 4[20]: %i\n", vec_count(test4), vec_capacity(test4), test4[20]);
    printf("3: %lu / %lu\n", vec_count(test3), vec_capacity(test3));

    vec_contact_vec(test3, test4);

    printf("3: %lu/%lu\n", vec_count(test3), vec_capacity(test3));
    printf("4: %lu/%lu\n", vec_count(test4), vec_capacity(test4));

    char** str_test = vec_new(char*, "a", "bc", "de");
    char* string = vec_concat_string(str_test);
    printf("str: %s\n", string);

    free(string);
    vec_destroy(test4);
    vec_destroy(test3);
}
# endif
