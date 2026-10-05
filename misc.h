# if 0 // testing
# define MISC_VECTOR
# define MISC_SYSTEM
# define MISC_BUILD
# define MISC_IMPLEMENTATIONS
# endif

# ifndef MISC_LIBRARY_IMPORT
# define MISC_LIBRARY_IMPORT

// dependencies
# ifdef MISC_BUILD
# define MISC_SYSTEM
# define MISC_VECTOR
# endif

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
    size_t capacity; // max amount of items that can be stored, NOT bytes
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

# define vec_from_array(array, count) vec_alloc(sizeof(array[0]), count)

/// All the vectors made in the way besides vec_new must be destroyed,
/// but due to their internal complexity regular free() won't work
# define vec_destroy(vector) (vector != NULL) ? free((MISC_Vector_Header*)(vector) - 1) : NULL

# define vec_ensure_alloc(vector) (vector != NULL) ? vec_ensure_alloc_func(vector, sizeof(*vector)) : NULL

/// Allocates the vector with specified size
/// NOTICE: vectors allocated by it MUST be destroyed using vec_destroy
/// returns NULL on allocation error
void* vec_alloc(size_t sizeof_value, size_t capacity);

/// Reallocates the vector with specified size
/// returns NULL on allocation error
void* vec_realloc(size_t sizeof_value, size_t new_capacity, void* vector);

/// Ensures the vector is allocated is allocated on heap, *reallocates* the vector
/// returns NULL on allocation error
void* vec_ensure_alloc_func(void* vector, size_t sizeof_value);

/// Concatinates a vector of strings into a single string, must be *freed* afterwards
/// returns NULL on allocation error
char* vec_concat_string(char** vector);

# ifdef MISC_IMPLEMENTATIONS

void* vec_alloc(size_t sizeof_value, size_t capacity) {
    MISC_Vector_Header* header = malloc(sizeof(MISC_Vector_Header) + sizeof_value * capacity);
    if (header == NULL) return NULL;
    header -> count = 0;
    header -> capacity = capacity;
    return header + 1;
}

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

void* vec_ensure_alloc_func(void* vector, size_t sizeof_value) {
    MISC_Vector_Header* header = (MISC_Vector_Header*)vector - 1;
    MISC_Vector_Header* new_header = realloc(header, header -> capacity * sizeof_value + sizeof(MISC_Vector_Header));
    if (new_header == NULL) return NULL;
    return new_header + 1;
};

# endif // MISC_IMPLEMENTATIONS
# endif // MISC_VECTOR

// NOTE: only linux systems is currently supported.
# ifdef MISC_SYSTEM
/// -1 for fork error, otherwise status code.
int sys_run_process(char* program, int argc, char** argv);

/// -1 for filesystem error, 0 for file not existing
int sys_get_file_edit_time(char* file);

# ifdef MISC_IMPLEMENTATIONS
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>
# include <sys/stat.h>
# include <errno.h>

int sys_run_process(char* program, int argc, char** argv) {
    int pid = fork();
    if (pid < 0) return -1;
    else if (pid > 0) {
        int status = 0;
        wait(&status);
        return status;
    }

    // child process
    char* arguments[argc + 1];
    memcpy(arguments, argv, argc);
    arguments[argc] = NULL; // NULL terminate the arguments list

    execvp(program, arguments);
    exit(0);
}

int sys_get_file_edit_time(char* file) {
    struct stat file_stat;
    if (stat(file, &file_stat) == -1) {
        if (errno == ENOENT) return 0;
        return -1;
    }

    return file_stat.st_mtime;
}

# endif // MISC_IMPLEMENTATIONS
# endif // MISC_SYSTEM

// NOTE: only linux systems is currently supported.
# ifdef MISC_BUILD
# include <stdint.h>
typedef struct {
    size_t length;   // length of the list of NULL terminated strings.
    size_t capacity; // maximum value the length field can become, NOT a byte count
    char*  cmd[];
} * BUILD_CMD_INNER; // inner type of the CMD used by user, needed for compiler to accept that every struct is the same

typedef BUILD_CMD_INNER* CMD; // vector type, but the one that is not supposed to be used by user

// After executing CMD the inner pointer shall be freed and replaced with NULL so it doesn't need to be freed afterwards

/// small hacky macro
# define cmd_new(...) (CMD)((BUILD_CMD_INNER []){cmd_new_func(sizeof((char* []) {__VA_ARGS__}) / sizeof(char*), (char* []) {__VA_ARGS__})})

// /// used as `rebuild_builder(vec_new(char*, "gcc", "%", "-O2", "-o", "%"), vec_new("build.c"), vec_new("headers/header_we_rely_on.h"))`
// /// arguments:
// ///
// # define rebuild_builder(cmd, sources, ...) 0

BUILD_CMD_INNER cmd_new_func(size_t length, char* values[]);

# ifdef MISC_IMPLEMENTATIONS
# include <string.h>

BUILD_CMD_INNER cmd_new_func(size_t length, char* values[]) {
    BUILD_CMD_INNER cmd = malloc(sizeof(size_t) * 2 + sizeof(char*) * length);
    cmd -> length   = length;
    cmd -> capacity = length;
    memcpy(cmd->cmd, values, length * sizeof(char*));
    return cmd;
}

# endif
# endif // MISC_BUILD

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
    CMD a = cmd_new("a", "b");
}
# endif
