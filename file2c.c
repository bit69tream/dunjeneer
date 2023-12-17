#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <libgen.h>
#include <string.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "usage: file2c <filepath>\n");
    return 1;
  }

  assert(argv[1]);
  FILE *fp = fopen(argv[1], "rb");

  char *array_name = basename(argv[1]);
  printf("const unsigned char ");

  char c_name[256] = {0};
  memset(c_name, 0, sizeof(c_name));

  for (size_t i = 0; i < strlen(array_name); i++) {
    if (!isalnum(array_name[i])) {
      c_name[i] = '_';
    } else {
      c_name[i] = array_name[i];
    }
  }

  printf("%s[] = {\n  ", c_name);

#define VALUES_PER_LINE 20
  unsigned long counter = 0;

  int byte = EOF;
  while ((byte = fgetc(fp)) != EOF) {
    assert(byte >= 0 && byte <= 255);

    printf("0x%02X,", byte);
    counter++;

    if (counter % VALUES_PER_LINE == 0) {
      printf("\n  ");
    } else {
      putchar(' ');
    }
  }

  if (counter % VALUES_PER_LINE != 0) {
    putchar('\n');
  }

  printf("};\n");

  printf("const unsigned long %s_size = %lu;\n", c_name, counter);

  fclose(fp);

  return 0;
}
