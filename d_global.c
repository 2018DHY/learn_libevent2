#include "event2/d_global.h"

#include <stdio.h>
#include <stdlib.h>


int64_t
dg_append(
	char **rt, char *src1, int64_t c1_len, char *src2, int64_t c2_len, bool free)
{
	int64_t sum = c1_len + c2_len;
	if (src1 != NULL && src2 != NULL) {
		(*rt) = (char *)malloc(sizeof(char) * sum);
		if (rt == NULL) {
			printf("dg_append>> malloc fail");
			return 0;
		}

		for (int i = 0; i < c1_len; i++) {
			(*rt)[i] = src1[i];
		}
		for (int i = 0; i < c2_len; i++) {
			int offset = i + c1_len;
			(*rt)[offset] = src2[i];
		}
	} else {
		return 0;
	}

	return sum;
}
void
dg_append_free(char *ptr)
{
	if (ptr != NULL) {
		free(ptr);
	}
}

int64_t
dg_read_all(char *path, char **rt)
{
	int unit = 4096;
	int count_unit = 1;
	int64_t size_rt = 0;
	int64_t cur_capacit = unit * count_unit;
	char *check = NULL;
	FILE *fl = fopen(path, "r");

	if (fl != NULL) {
		printf("file open succese\n");
		(*rt) = malloc(sizeof(char) * cur_capacit);
		char ch;
		while (1) {
			ch = getc(fl);
			if (ch != EOF) {
				if (size_rt < cur_capacit) {
					(*rt)[size_rt] = ch;
				} else {
					count_unit++;
					cur_capacit = unit * count_unit;
					check = realloc((*rt), cur_capacit);
					if (check == NULL) {
						if ((*rt) != NULL) {
							free((*rt));
							return 0;
						}
						return 0;
					}
					(*rt) = check;
					(*rt)[size_rt] = ch;
				}
				size_rt++;
			} else {
				break;
			}
		}
		fclose(fl);
	} else {
		printf("dg_read_all>>open file failed");
		(*rt) = NULL;
		return 0;
	}

	return size_rt;
}

void
dg_read_all_free(char *ptr)
{
	if (ptr != NULL) {
		free(ptr);
	}
}


int64_t
f_read_all(const char *const path, char **rt)
{
	FILE *fd = fopen(path, "rb");
	if (fd == NULL)
		return 0;
	fseek(fd, 0, SEEK_END);
	int64_t len = ftello(fd);
	fseek(fd, 0, SEEK_SET);
	*rt = (char *)malloc(sizeof(char) * len);
	if (*rt == NULL) {
		printf("f_read_all>>bad malloc");
		return 0;
	}
	if (fread(*rt, 1, len, fd) <= 0) {
		fclose(fd);
		*rt = NULL;
		return 0;
	}
	fclose(fd);
	return len;
}

void
f_read_all_free(char *ptr)
{
	if (ptr != NULL) {
		free(ptr);
	}
}