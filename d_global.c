#include "event2/d_global.h"

#include <stdio.h>
#include <stdlib.h>


int
dg_append(char **rt, char *src1, int c1_len, char *src2, int c2_len,bool free)
{
	int sum = c1_len + c2_len;
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
	free(ptr);
}

int
dg_read_all(char *path, char **rt)
{
	int unit = 4096;
	int count_unit = 1;
	int size_rt = 0;
	int cur_capacit = unit * count_unit;
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
					(*rt)=check;
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
	free(ptr);
}