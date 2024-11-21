#include <stdio.h>

// Function prototypes
void process_one_problem(int count);
void parse_connection(char *buf);

void process_one_problem(int count) {
    char buf[100];

    for (int i = 0; i < count; i++) {
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            break;
        }
        parse_connection(buf);
    }
}

void parse_connection(char *buf) {
    char from[18], to[18];
    int dpt[2], arv[2], fare;

    sscanf(buf, "%s %d:%d %s %d:%d %d", 
                from, &(dpt[0]), &(dpt[1]), to, &(arv[0]), &(arv[1]), &fare);
    printf("Connection: %s %02d:%02d %s %02d:%02d %d\n", from, dpt[0], dpt[1], to, arv[0], arv[1], fare);
}

int main() {
    int count;
    char buf[100];

    // Read the number of connections
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        sscanf(buf, "%d", &count);

        // Check for end of input
        if (count == 0) {
            break;
        }

        process_one_problem(count);
    }
    return 0;
}