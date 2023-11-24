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
  printf("static const unsigned char ");

  for (size_t i = 0; i < strlen(array_name); i++) {
    if (!isalnum(array_name[i])) {
      putchar('_');
    } else {
      putchar(array_name[i]);
    }
  }

  printf("[] = {\n  ");

  #define VALUES_PER_LINE 20
  int counter = 0;

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

  fclose(fp);

  return 0;
}
