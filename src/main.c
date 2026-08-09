
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

void front(char*);

char* load_file(char*);

int main(int argc, char** argv) {

    char* source;

    for(size_t i = 1; i < argc; i++) {

        source = load_file(argv[i]);

        front(source);

        free(source);

    }
    return 0;

}

char* load_file(char* filename) {

    char *source = malloc(sizeof(char) * 256);
    size_t len = 256;
    size_t size = 0;

    FILE* fp = fopen(filename, "r");

    char c;
    for(; (c = fgetc(fp)) != EOF;) {

        if(size >= len) {
            len *= 2;
            source = realloc(source, len);
        }

        source[size] = c;
        size++;

    }

    fclose(fp);

    source = realloc(source, size+1);
    source[size] = '\0';

    return source;

}
