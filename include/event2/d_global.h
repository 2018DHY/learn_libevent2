#ifndef D_GLOBAL
#define D_GLOBAL
#include <event2/visibility.h>
#include <stdbool.h>

/**
 * 返回两个字符串拼接后的大小,若为0则失败
 */
EVENT2_EXPORT_SYMBOL
int dg_append(char **rt, char *src1, int c1_len, char *src2, int c2_len,bool free);
EVENT2_EXPORT_SYMBOL
void dg_append_free(char *ptr);
EVENT2_EXPORT_SYMBOL
int dg_read_all(char *path, char **rt);
EVENT2_EXPORT_SYMBOL
void dg_read_all_free(char *ptr);

#endif