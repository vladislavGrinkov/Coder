#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct Dictionary {
	struct Map **words;
	int count;
};

struct Map {
	char *key;
	int value;
	int code;
};
// добавление слов в словарь
void addToDictionary(struct Dictionary *dict, char *word) {
	int i, isNewWord = 1;
	for (i = 0; i < dict->count; i++) {
		if (strcmp(word, dict->words[i]->key) == 0) {
			dict->words[i]->value++;
			isNewWord = 0;
			break;
		}
	}

	if (isNewWord) {
		dict->count++;
		dict->words = realloc(dict->words, dict->count * sizeof(struct Map *));
		dict->words[dict->count - 1] = malloc(sizeof(struct Map));
		dict->words[dict->count - 1]->key = word;
		dict->words[dict->count - 1]->value = 1;
		dict->words[dict->count - 1]->code = 0;
	}
}

int isLetter(char c) {
	if (!isspace((unsigned char)c) && (!ispunct((unsigned char)c) || c == '\'' || c == '`') && c != '#' && c != '/')
		return 1;
	else
		return 0;
}

struct Dictionary *createCodes(char* path) {
	FILE *in = fopen(path, "r");

	if (in != NULL) {
		FILE *out = fopen("codes.txt", "w");

		if (out != NULL) {
			int i, code = 0;
			char c;
			char *word = malloc(sizeof(char) * 50);
			struct Dictionary *dict = malloc(sizeof(struct Dictionary));

			word[0] = '\0';

			dict->words = NULL;
			dict->count = 0;

			while (c = fgetc(in)) {
				if (feof(in))
					break;

				if (isLetter(c)) {
					word[strlen(word) + 1] = '\0';
					word[strlen(word)] = c;
				}
				else if (strlen(word) > 0) {
					addToDictionary(dict, word);

					word = malloc(sizeof(char) * 50);
					word[0] = '\0';
				}
			}

			for (int i = 0; i < dict->count; i++)
			{
				if (dict->words[i]->value > 2) {
					code++;
					if (code == 26 || code == 255 + 26 || code == 255 * 2 + 26 || code == 35 || code == 255 + 35 || code == 255 * 2 + 35)
						code++;
					dict->words[i]->code = code;
				}
			}

			fprintf(out, "Слово\t\t\tповторения\tкод\n");

			for (int i = 0; i < dict->count; i++) {
				if (strlen(dict->words[i]->key) > 15) {
					fprintf(out, "%s\t%d\t\t%d\n", dict->words[i]->key, dict->words[i]->value, dict->words[i]->code);
				}
				else if (strlen(dict->words[i]->key) > 7) {
					fprintf(out, "%s\t\t%d\t\t%d\n", dict->words[i]->key, dict->words[i]->value, dict->words[i]->code);
				}
				else {
					fprintf(out, "%s\t\t\t%d\t\t%d\n", dict->words[i]->key, dict->words[i]->value, dict->words[i]->code);
				}
			}

			fclose(in);
			fclose(out);

			return dict;
		}
		else {
			fclose(in);
			printf("Error. Input file not found");
			return NULL;
		}
	}
	else {
		printf("Error. Input file not found");
		return NULL;
	}
}

int getCodeByWord(char *word, struct Dictionary *dict) {
	for (int i = 0; i < dict->count; i++)
		if (strcmp(word, dict->words[i]->key) == 0)
			return dict->words[i]->code;
}

char *getWordByCode(int code, struct Dictionary *dict) {
	for (int i = 0; i < dict->count; i++)
		if (code == dict->words[i]->code)
			return dict->words[i]->key;
	return NULL;
}

void encode(char *path, struct Dictionary *dict) {
	FILE *in = fopen(path, "r");
	int c;
	int i;
	char *word = malloc(sizeof(char) * 20);
	word[0] = '\0';

	if (in != NULL) {
		FILE *out = fopen("encode.txt", "w");

		if (out != NULL) {
			while (c = fgetc(in)) {
				if (feof(in))
					break;

				if (isLetter(c)) {
					word[strlen(word) + 1] = '\0';
					word[strlen(word)] = c;
				}
				else if (strlen(word) > 0) {
					int code = getCodeByWord(word, dict);
					if (code != 0) {
						while (code > 255) {
							fprintf(out, "#");
							code -= 255;
						}
						fprintf(out, "#%c%c", code, c);
					}
					else
						fprintf(out, "%s%c", word, c);
					word = malloc(sizeof(char) * 20);
					word[0] = '\0';
				}
				else if (c == '#' || c == '/')
					fprintf(out, "/%c", c);
				else if (c != -1)
					fprintf(out, "%c", c);
			}
			fclose(in);
			fclose(out);
		}
		else {
			fclose(in);
			printf("Error. Input file not found");
		}
	}
	else {
		printf("Error. Input file not found");
	}
}

void decode(char *path, struct Dictionary *dict) {
	FILE *in = fopen(path, "r");
	unsigned int c;
	int i;
	char *word = malloc(sizeof(char) * 20);
	char *decodeWord = NULL;
	word[0] = '\0';

	if (in != NULL) {
		FILE *out = fopen("decode.txt", "w");

		if (out != NULL) {
			while (c = fgetc(in)) {
				if (feof(in))
					break;
				if (c == '#') {
					i = 0;
					while (c == '#') {
						c = fgetc(in);
						i++;
					}
					fprintf(out, "%s", getWordByCode((i - 1) * 255 + c, dict));
				}
				else if (c == '/') {
					c = fgetc(in);
					fprintf(out, "%c", c);
				}
				else if (c != -1) {
					fprintf(out, "%c", c);
				}
			}
			fclose(in);
			fclose(out);
		}
		else {
			fclose(in);
			printf("Error. Input file not found");
		}
	}
	else {
		printf("Error. Input file not found");
	}
}

int main() {
	struct Dictionary *dict = createCodes("input.txt");
	if (dict != NULL) {
		encode("input.txt", dict);
		decode("encode.txt", dict);
	}
	return 0;
}