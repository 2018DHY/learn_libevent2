#ifndef D_GLOBAL
#define D_GLOBAL
#include <event2/visibility.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



struct byt_buf_ary
{
    int64_t size;
    int64_t capacity;
    char* dt;
};

typedef struct byt_buf_ary bba;

EVENT2_EXPORT_SYMBOL
void scale_buf(bba** buf); 
EVENT2_EXPORT_SYMBOL
void buf_ary_free(bba** buf);

/**
 * 返回两个字符串拼接后的大小,若为0则失败
 * rt为拼接后的字符串指针
 * src*为需要拼接的两个字符串
 * *_len是两个字符串的长度
 * free是代表是否要帮你释放src1（当前无用）
 */
EVENT2_EXPORT_SYMBOL
int64_t dg_append(char **rt, char *src1, int64_t c1_len, char *src2, int64_t c2_len,bool free);
EVENT2_EXPORT_SYMBOL
void dg_append_free(char *ptr);
/**
 * 以二进制方式将读取文件所有内容
 * path为文件路径
 * rt为文件所有内容
 */
EVENT2_EXPORT_SYMBOL
int64_t dg_read_all(char *path, char **rt);
EVENT2_EXPORT_SYMBOL
void dg_read_all_free(char *ptr);
EVENT2_EXPORT_SYMBOL

/**
 * 以二进制方式读取所有内容 不同实现
 * path为文件路径
 * rt为文件所有内容
 */
int64_t f_read_all(const char* const path,char** rt);
EVENT2_EXPORT_SYMBOL
void f_read_all_free(char* ptr);



#endif