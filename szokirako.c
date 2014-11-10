/* Szokirarako program */
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BOARD_SIZE 15
#define LETTER_NUM 7

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"


/* Board fields */
#define L2 1
#define L3 2
#define W2 3
#define W3 4
#define K  5

#define MACHINE 0
#define HUMAN 1

enum word_direction {
	RIGHT,
	DOWN
};

typedef struct _dict {
	int len;
	char word[LETTER_NUM];
	struct _dict *next;
} dict;

typedef struct _permutes {
	char w[LETTER_NUM];
	struct _permutes *next;
} permutes;

typedef struct {
	int score;
	char letters[LETTER_NUM];
} player_t;

player_t players[2];

typedef struct {
	const char letter;
	int point;
	const int max_num;
	int act_num;
} letter_t;

typedef struct {
	int x;
	int y;
} cordinate_t;

letter_t letter_set[27] = {
	{'e', 1, 12, 12},
	{'a', 1, 9, 9},
	{'i', 1, 9, 9},
	{'o', 1, 8, 8},
	{'n', 1, 6, 6},
	{'r', 1, 6, 6},
	{'t', 1, 6, 6},
	{'l', 1, 4, 4},
	{'s', 1, 4, 4},
	{'u', 1, 4, 4},
	{'d', 2, 4, 4},
	{'g', 2, 3, 3},
	{'b', 3, 2, 2},
	{'c', 3, 2, 2},
	{'m', 3, 2, 2},
	{'p', 3, 2, 2},
	{'f', 4, 2, 2},
	{'h', 4, 2, 2},
	{'v', 4, 2, 2},
	{'w', 4, 2, 2},
	{'y', 4, 2, 2},
	{'k', 5, 1, 1},
	{'j', 8, 1, 1},
	{'x', 8, 1, 1},
	{'q', 10,1, 1},
	{'z', 10,1, 1},
	{'#', 0, 2, 2}
};

char board_play[15][15];
const int board_schema[15][15] = {
	{W3,0 ,0 ,L2,0 ,0 ,0 ,W3,0 ,0 ,0 ,L2,0 ,0 ,W3},
	{0 ,W2,0 ,0 ,0, L3,0, 0, 0, L3,0, 0, 0, W2,0 },
	{0, 0, W2,0, 0, 0, L2,0, L2,0, 0, 0, W2,0, 0 },
	{L2,0, 0, W2,0, 0, 0, L2,0, 0, 0, W2,0, 0, L2},
	{0, 0, 0, 0, W2,0, 0, 0, 0, 0, W2,0, 0, 0, 0 },
	{0, L3,0, 0, 0, L3,0, 0, 0, L3,0, 0, 0, L3,0 },
	{0, 0, L2,0, 0, 0, L2,0, L2,0, 0, 0, L2,0, 0 },
	{W3,0, 0, L2,0, 0, 0, K, 0, 0, 0, L2,0, 0, W3},
	{0, 0, L2,0, 0, 0, L2,0, L2,0, 0, 0, L2,0, 0 },
	{0, L3,0, 0, 0, L3,0, 0, 0, L3,0, 0, 0, L3,0 },
	{0, 0, 0, 0, W2,0, 0, 0, 0, 0, W2,0, 0, 0, 0 },
	{L2,0, 0, W2,0, 0, 0, L2,0, 0, 0, W2,0, 0, L2},
	{0, 0, W2,0, 0, 0, L2,0, L2,0, 0, 0, W2,0, 0 },
	{0 ,W2,0 ,0 ,0, L3,0, 0, 0, L3,0, 0, 0, W2,0 },
	{W3,0 ,0 ,L2,0 ,0 ,0 ,W3,0 ,0 ,0 ,L2,0 ,0 ,W3}
};

permutes *perm_start, *perm_head;
dict *dict_start, *dict_head;

void swap(char *a, char *b) {
	char tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void permute(char *a, int i, int n) {
	int j;
	if (i == n) {
		strcpy(perm_head -> w, a);
		perm_head -> next = malloc(sizeof(permutes));
		perm_head = perm_head -> next;
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

void read_dict() {
	FILE *f = fopen("twl06.txt", "r");
	char tmp[128];
	int len;
	dict_start = malloc(sizeof(dict));
	dict_head = dict_start;
	while (fgets(tmp, 128, f) != NULL) {
		len = strlen(tmp);
		if (len > 7) continue;
		strcpy(dict_head -> word, tmp);
		dict_head -> len = len;
		dict_head -> next = malloc(sizeof(dict));
		dict_head = dict_head -> next;
	}
	dict_head = NULL;
	fclose(f);
}

/* Find the all possible word */
void possible_words() {
	dict *i;
	permutes *j;
	int cnt =0;
	for (j = perm_start; j -> next != NULL; j = j -> next) {
		for (i = dict_start; i -> next != NULL; i = i -> next) {
			if (strcmp(j->w, i->word) == 0) {
				printf("%s", i->word);
				printf("%s %s", j->w, i->word);
			}
			cnt++;
		}
	}
	printf("%d\n", cnt);
}

void free_data() {
	dict *i, *ni;
	permutes *j, *nj;
	for (i = dict_start; i -> next != NULL; i = ni) {
		ni = i -> next;
		free(i);
	}
	for (j = perm_start; j -> next != NULL; j = nj) {
		nj = j -> next;
		free(j);
	}
}

void init_board() {
	int i, j;
	for (i = 0; i<15; i++){
		for (j = 0; j<15; j++) {
			board_play[i][j] = ' ';
		}
	}
}
void print_board() {
	int i, j, k;
	for (i = 0; i < BOARD_SIZE; i++){
		if (i == 0) {
			for (k = 0; k < BOARD_SIZE; k++) {
				printf("====");
			}
		}
		else {
			for (k = 0; k < BOARD_SIZE; k++) {
				if (k == 0) printf("¤");
				printf("---¤");
			}
		}
		printf("\n");
		printf("|");
		for (j = 0; j < BOARD_SIZE; j++) {
			printf(KMAG " %c " RESET "|", board_play[i][j]);
		}
		printf("[]\n");
	}
}

int get_letter_score(char l) {
	int i;
	for (i = 0; i < 26 ; i++) {
		if (letter_set[i].letter == l) return letter_set[i].point;
	}
	return 0;
}

#define SCORE(x,y)							\
	switch (board_schema[x][y]) {					\
	case L2: score += get_letter_score(board_play[x][y]) * 2; break; \
	case L3: score += get_letter_score(board_play[x][y]) * 3; break; \
	case W2: multi *= 2;						\
		score += get_letter_score(board_play[x][y]) * 3; break;	\
	case W3: multi *= 3;						\
		score += get_letter_score(board_play[x][y]); break;	\
	default: get_letter_score(board_play[x][y]); break;		\
	}

void calculate_word_score(cordinate_t start, enum word_direction wd, int player) {
	int i, j = 0;
	int score = 0, multi = 1;
	if (wd == RIGHT) {
		for (i = start.y; board_play[start.x][i] != '0' && i < BOARD_SIZE; i++) {
			j++;
			SCORE(start.x, i);
		}
	}
	else {
		for (i = start.x; board_play[i][start.y] != '0' && i < BOARD_SIZE; i++) {
			j++;
			SCORE(i, start.y);
		}
	}
	score *= multi;
	players[player].score += score;
}
void take_word(cordinate_t start, enum word_direction wd, char *word) {
	int i;
	int len = strlen(word);
	for (i = 0; i < len; i++) {
		if (wd == RIGHT) {
			board_play[start.x][start.y+i] = word[i];
		}
		else {
			board_play[start.x+i][start.y] = word[i];
		}
	}
}

void init_players() {
	players[MACHINE].score = 0;
	players[HUMAN].score = 0;
}

int main() {
	char a[LETTER_NUM];
	cordinate_t c;
	new_char_set(a);
	printf("%s\n", a);
	perm_start = malloc(sizeof(permutes));
	perm_head = perm_start;
	permute (a, 0, LETTER_NUM-1);
	read_dict();
	possible_words();
	init_board();
	init_players();
	c.x = 0;
	c.y = 0;
	take_word(c, RIGHT, "fosfalo");
	calculate_word_score(c, RIGHT, HUMAN);
	print_board();
	printf("score: %d\n", players[HUMAN].score);
	free_data();

	return 0;
}
