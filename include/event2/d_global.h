#ifndef D_GLOBAL
#define D_GLOBAL

int dg_append(char *rt, char *src1, int c1_len, char *src2, int c2_len);
void dg_append_free(char *ptr);
int dg_read_all(char *path, char *rt);
void dg_read_all_free(char *ptr);

#endif