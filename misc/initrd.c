#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct initrd_header {
  uint8_t magic;
  uint8_t name[64];
  uint32_t offset;
  uint32_t length;
};

int
main(int argc, char **argv) {
  uint32_t nheaders = (argc - 1) / 2;
  struct initrd_header headers[64];
  printf("size of header: %d\n", sizeof(struct initrd_header));
  uint32_t off = sizeof(struct initrd_header) * 64 + sizeof(uint32_t);

  int i;
  for(i = 0; i < nheaders; i++) {
    printf("writing file %s->%s at 0x%x\n", argv[i * 2 + 1], argv[i * 2 + 2],
	   off);
    strcpy(headers[i].name, argv[i * 2 + 2]);
    headers[i].offset = off;
    FILE *stream = fopen(argv[i * 2 + 1], "r");
    if(stream == 0) {
      printf("Error: file not found: %s\n", argv[i * 2 + 1]);
      return 1;
    }
    fseek(stream, 0, SEEK_END);
    headers[i].length = ftell(stream);
    off += headers[i].length;
    fclose(stream);
    headers[i].magic = 0xBF;
  }

  FILE *wstream = fopen("./initrd.img", "w");
  uint8_t *data = (uint8_t *) malloc(off);
  fwrite(&nheaders, sizeof(uint32_t), 1, wstream);
  fwrite(headers, sizeof(struct initrd_header), 64, wstream);

  for(i = 0; i < nheaders; i++) {
    FILE *stream = fopen(argv[i * 2 + 1], "r");
    uint8_t *buf = (uint8_t *) malloc(headers[i].length);
    size_t readed = fread(buf, 1, headers[i].length, stream);
    size_t writed = fwrite(buf, 1, headers[i].length, wstream);
    printf("read=%d write=%d offset=%d buf=%s buf[0]=%c\n", readed, writed,
	   headers[i].offset, buf, buf[0]);
    fclose(stream);
    free(buf);
  }

  fclose(wstream);
  free(data);

  return 0;
}
