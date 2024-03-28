/*
 * Copyright (c) 2021, Ada Logics Ltd. 
 * All rights reserverd. 
 * 
*/

#include <unistd.h>

// Simple garbage collector 
#define GB_SIZE 100

void *pointer_arr[GB_SIZE];
static int pointer_idx = 0;

// If the garbage collector is used then this must be called as first thing
// during a fuzz run.
void af_gb_init() {
  pointer_idx = 0;

   for (int i = 0; i < GB_SIZE; i++) {
     pointer_arr[i] = NULL;
   }
}

void af_gb_cleanup() {
  for(int i = 0; i < GB_SIZE; i++) {
    if (pointer_arr[i] != NULL) {
      free(pointer_arr[i]);
    }
  }
}

char *af_get_null_terminated(const uint8_t **data, size_t *size) {
#define STR_SIZE 75
  if (*size < STR_SIZE || (int)*size < 0) {
    return NULL;
  }

  char *new_s = malloc(STR_SIZE + 1);
  memcpy(new_s, *data, STR_SIZE);
  new_s[STR_SIZE] = '\0';

  *data = *data+STR_SIZE;
  *size -= STR_SIZE;
  return new_s;
}

char *af_gb_get_random_data(const uint8_t **data, size_t *size, size_t to_get) {
  if (*size < to_get || (int)*size < 0) {
    return NULL;
  }

  char *new_s = malloc(to_get);
  memcpy(new_s, *data, to_get);

  pointer_arr[pointer_idx++] = (void*)new_s;
  
  *data = *data + to_get;
  *size -= to_get;

  return new_s;
}

char *af_gb_get_null_terminated(const uint8_t **data, size_t *size) {

  char *nstr = af_get_null_terminated(data, size);
  if (nstr == NULL) {
    return NULL;
  }
  pointer_arr[pointer_idx++] = (void*)nstr;
  return nstr;
}

char *af_gb_alloc_data(size_t len) {
  char *ptr = calloc(1, len);
  pointer_arr[pointer_idx++] = (void*)ptr;
  
  return ptr;
}

char *af_gb_get_fixed_string() {
  char *ptr = malloc(2);
  ptr[0] = 'A';
  ptr[1] = '\0';
  pointer_arr[pointer_idx++] = (void*)ptr;

  return ptr;
}

short af_get_short(const uint8_t **data, size_t *size) {
  if (*size <= 0) return 0;
  short c = (short)(*data)[0];
  *data += 1;
  *size-=1;
  return c;
}

int af_get_int(const uint8_t **data, size_t *size) {
  if (*size <= 4) return 0;
  const uint8_t *ptr = *data;
  int val = *((int*)ptr);
  *data += 4;
  *size -= 4;
  return val;
}


// end simple garbage collector.


/* A-style */
const uint8_t *a_origin_data;
size_t a_size;

void af_safe_gb_init(const uint8_t *data, size_t size) {
  af_gb_init();
  a_origin_data = data;
  a_size = size;
}

int ada_safe_get_int() {
  return af_get_int(&a_origin_data, &a_size);
}

char *ada_safe_get_char_p() {
  char *tmps = af_gb_get_null_terminated(&a_origin_data, &a_size);
  if (tmps != NULL) {
    return tmps;
  }
  return af_gb_get_fixed_string();
}

char *filename2 = NULL;

char *af_safe_write_random_file() {
  char *filename = malloc(10);
  filename[0] = '/';
  filename[1] = 't';
  filename[2] = 'm';
  filename[3] = 'p';
  filename[4] = '/';
  filename[5] = '1';
  filename[6] = '2';
  filename[7] = '.';
  filename[8] = 'a';
  filename[9] = '\0';
  filename2 = filename;

  FILE *fp = fopen(filename, "wb");
  char *content = ada_safe_get_char_p();
  fwrite(content, strlen(content), 1, fp);
  fclose(fp);

  return filename;
}

void af_safe_gb_cleanup() {
  af_gb_cleanup();

  if (filename2 != NULL) {
    unlink(filename2);
    free(filename2);
    filename2 = NULL;
  }
}
