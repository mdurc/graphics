#include "io.h"

#include <errno.h>
#include <stdio.h>

#include "../c-lib/misc.h"

#define IO_READ_CHUNK_SIZE  2097152
#define IO_READ_ERROR_GENERAL "Error reading file %s, errno: %d\n"
#define IO_READ_ERROR_MEMORY "Not enough free memory to read file: %s\n"

file_t io_file_read(const char* path) {
  file_t file = {.is_valid = false};

  FILE* fp = fopen(path, "rb");
  if (!fp) {
    ERROR_RETURN(file, IO_READ_ERROR_GENERAL, path, errno);
  }

  char* data = NULL;
  char* tmp;
  size_t used = 0;
  size_t size = 0;
  size_t n;

  while (true) {
    if (used + IO_READ_CHUNK_SIZE + 1 > size) {
      size = used + IO_READ_CHUNK_SIZE + 1;

      if (size <= used) {
        free(data);
        ERROR_RETURN(file, "Input file too large: %s\n", path);
      }

      tmp = realloc(data, size);
      if (!tmp) {
        free(data);
        ERROR_RETURN(file, IO_READ_ERROR_MEMORY, path);
      }
      data = tmp;
    }

    n = fread(data + used, 1, IO_READ_CHUNK_SIZE, fp);
    if (n == 0) break;

    used += n;
  }

  if (ferror(fp)) {
    free(data);
    ERROR_RETURN(file, IO_READ_ERROR_GENERAL, path, errno);
  }

  tmp = realloc(data, used + 1);
  if (!tmp) {
    free(data);
    ERROR_RETURN(file, IO_READ_ERROR_MEMORY, path);
  }
  data = tmp;
  data[used] = 0;

  file.data = data;
  file.len = used;
  file.is_valid = true;

  fclose(fp);

  LOG("Successfully read from file %s", path);

  return file;
}

int io_file_write(void* buf, size_t size, const char* path) {
  FILE* fp = fopen(path, "wb");
  if (!fp) {
    ERROR_RETURN(1, "Cannot write file: %s.\n", path);
  }

  size_t chunks_written = fwrite(buf, size, 1, fp);

  fclose(fp);

  if (chunks_written != 1) {
    ERROR_RETURN(1, "Write error: %zu chunks\n", chunks_written);
  }

  LOG("Successfully wrote from file %s", path);

  return 0;
}
