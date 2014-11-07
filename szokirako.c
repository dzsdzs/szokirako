/* Szokirarako program */
#include <time.h>
#include <stdio.h>

#define LETTER_NUM 7

void swap(char *a, char *b) {
	char tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void permute(char *a, int i, int n) {
	int j;
	static int cnt = 0;
	if (i == n) {
		printf("%s, %d\n", a, ++cnt);
	}
	else {
		for (j = i; j <= n; j++) {
			swap(a+i, a+j);
			permute(a, i+1, n);
			swap(a+i, a+j);
		}
	}
}

void new_char_set(char *set) {
	int i;
	srand(time(NULL));
	for (i = 0; i < LETTER_NUM; i++) {
		set[i] = 'a' + (rand() % 26);
	}
}
int main() {
	char a[LETTER_NUM];
	new_char_set(a);
	permute (a, 0, LETTER_NUM-1);
	return 0;
}
