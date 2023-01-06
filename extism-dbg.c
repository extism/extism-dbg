#include <assert.h>
#include <extism.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static uint8_t *read_stdin(size_t *len);
static uint8_t *read_file(const char *filename, size_t *len);

#define BAIL(msg, ...)                                                         \
  fprintf(stderr, "ERROR " msg "\n", __VA_ARGS__);                             \
  ret = 1;                                                                     \
  goto cleanup;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fputs(
        "usage: extism-dbg <Manifest or WebAssembly file> <function> [input]\n",
        stderr);
    return 1;
  }

  int ret = 0;                                     // Return value
  size_t wasmLength = 0;                           // Length of WASM data
  uint8_t *wasm = read_file(argv[1], &wasmLength); // Alloc wasm
  if (wasm == NULL) {
    fprintf(stderr, "ERROR Unable to read WebAssembly file or Manifest: %s\n",
            argv[1]);
    return 1;
  }

  bool inputFromFile =
      false;             // Set to true if `data` is read from a file (or stdin)
  uint8_t *data = NULL;  // Input data
  size_t dataLength = 0; // Input data length

  // Create context
  ExtismContext *ctx = extism_context_new(); // Alloc ctx

  // Create plugin
  setenv("EXTISM_DEBUG", "1", 0);
  ExtismPlugin plugin =
      extism_plugin_new(ctx, wasm, wasmLength, true); // Alloc plugin
  free(wasm);                                         // Free wasm
  if (plugin < 0) {
    BAIL("%s", extism_error(ctx, plugin));
  }

  // Make sure that the specified function exists before reading input
  if (!extism_plugin_function_exists(ctx, plugin, argv[2])) {
    BAIL("Function does not exist in plugin: %s", argv[2]);
  }

  // If there are 4 arguments then use the final argument as input
  if (argc > 3) {
    if (argv[3][0] == '@') {
      // @input.txt reads from the file input.txt
      inputFromFile = true;
      data = read_file(argv[3] + 1, &dataLength); // Alloc data
      if (data == NULL) {
        BAIL("Unable to read file: %s", argv[3]);
      }
    } else if (strlen(argv[3]) == 1 && argv[3][0] == '-') {
      // - reads from stdin
      inputFromFile = true;
      data = read_stdin(&dataLength);
    } else {
      // use the argument as-is
      data = (uint8_t *)argv[3];
      dataLength = strlen(argv[3]);
    }
  }

  // Call the function
  uint32_t rc = extism_plugin_call(ctx, plugin, argv[2], data, dataLength);
  if (rc != 0) {
    BAIL("%s", extism_error(ctx, plugin));
  } else {
    // Handle error
    const uint8_t *output = extism_plugin_output_data(ctx, plugin);
    size_t outputLength = extism_plugin_output_length(ctx, plugin);
    write(STDOUT_FILENO, output, outputLength);
  }

cleanup:
  // Data should only be freed if it was read from a file (or stdin)
  if (data != NULL && inputFromFile) {
    free(data); // Free data
  }

  extism_plugin_free(ctx, plugin); // Free plugin
  extism_context_free(ctx);        // Free ctx
  return ret;
}

static uint8_t *read_stdin(size_t *len) {
  size_t size = 4096;
  uint8_t *out = malloc(size);
  if (out == NULL)
    return NULL;

  int ch;
  size_t index = 0;

  while ((ch = getchar()) != EOF) {
    if (index >= size) {
      size += 1024;
      out = realloc(out, size);
      assert(out != NULL);
    }
    out[index++] = ch;
  }

  *len = index;
  return out;
}

static uint8_t *read_file(const char *filename, size_t *len) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  size_t length = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  uint8_t *data = malloc(length);
  if (data == NULL) {
    return NULL;
  }

  assert(fread(data, 1, length, fp) == length);
  fclose(fp);

  *len = length;
  return data;
}
