#include <assert.h>
#include <extism.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

uint8_t *read_file(const char *filename, size_t *len) {
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

#define BAIL(msg, ...)                                                         \
  fprintf(stderr, msg "\n", __VA_ARGS__);                                      \
  ret = 1;                                                                     \
  goto cleanup;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fputs("usage: extism-dbg <Manifest or WebAssembly file> <function> [input "
          "file]\n",
          stderr);
    return 1;
  }

  int ret = 0;
  size_t wasmLength = 0;
  uint8_t *wasm = read_file(argv[1], &wasmLength);
  if (wasm == NULL) {
    fprintf(stderr, "Unable to read file: %s\n", argv[1]);
    return 1;
  }

  uint8_t *data = NULL;
  size_t dataLength = 0;
  ExtismContext *ctx = extism_context_new();

  setenv("EXTISM_DEBUG", "1", 0);
  ExtismPlugin plugin = extism_plugin_new(ctx, wasm, wasmLength, NULL, 0, true);
  free(wasm);
  if (plugin < 0) {
    BAIL("%s", extism_error(ctx, plugin));
  }

  bool inputFromFile = false;

  if (argc > 3) {
    if (argv[3][0] == '@') {
      inputFromFile = true;
      data = read_file(argv[3] + 1, &dataLength);
      if (data == NULL) {
        BAIL("Unable to read file: %s", argv[3]);
      }
    } else {
      data = (uint8_t *)argv[3];
      dataLength = strlen(argv[3]);
    }
  }

  uint32_t rc = extism_plugin_call(ctx, plugin, argv[2], data, dataLength);
  if (rc != 0) {
    BAIL("%s", extism_error(ctx, plugin));
  } else {
    const uint8_t *output = extism_plugin_output_data(ctx, plugin);
    size_t outputLength = extism_plugin_output_length(ctx, plugin);
    write(STDOUT_FILENO, output, outputLength);
  }

cleanup:
  if (data != NULL && inputFromFile) {
    free(data);
  }

  extism_plugin_free(ctx, plugin);
  extism_context_free(ctx);
  return ret;
}
