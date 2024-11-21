#include <stdio.h>
#include <string.h>

#define MAXCITY 100

char city_name[MAXCITY][18];
int ncity;

#define MAXCONN 2000+1

struct train {
    int from, to;
    int dpt, arv;
    int fare;
} trains[MAXCONN], rtrains[MAXCONN];

int nconn;

#define HAKODATE 0
#define TOKYO 1

#define INFINITE 99999999

int from_hakodate[MAXCITY][MAXCONN],
    from_tokyo[MAXCITY][MAXCONN],
    to_hakodate[MAXCITY][MAXCONN],
    to_tokyo[MAXCITY][MAXCONN];

#define BIAS ((18-8)*60);

// Function prototypes
void process_one_problem(int count);
void parse_connection(char *buf);
int city_id(char *name);
void print_train_array(struct train *trains);
void cleanup(void);

// Task 1
struct train find_most_expensive_train(void);
struct train find_longest_duration_train(void);
struct train* find_trains_from_station(char *station_name);
struct train* find_trains_to_station(char *station_name);

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
    // printf("Connection: %s %02d:%02d %s %02d:%02d %d\n", from, dpt[0], dpt[1], to, arv[0], arv[1], fare);

    trains[nconn].from = city_id(from);
    trains[nconn].to = city_id(to);
    trains[nconn].dpt = dpt[0] * 60 + dpt[1];
    trains[nconn].arv = arv[0] * 60 + arv[1];
    trains[nconn].fare = fare;
    nconn++;
    // printf("Station number of %s: %d\n", from, trains[nconn-1].from);
}

int city_id(char *name) {
    for (int i = 0; i < ncity; i++) {
        if (strcmp(name, &(city_name[i][0])) == 0) {
            return i;
        }
    }
    strcpy(&(city_name[ncity][0]), name);
    return ncity++;
}

// If multiple trains have the highest fare, the function returns the first occurrence.  
struct train find_most_expensive_train(void) {
    struct train most_expensive_train = trains[0];

    for (int i = 1; i < nconn; i++) {
        if (trains[i].fare > most_expensive_train.fare) {
            most_expensive_train = trains[i];
        }
    }

    return most_expensive_train;
}

// If multiple trains have the longest duration, the function returns the first occurrence.  
struct train find_longest_duration_train(void) {
    struct train longest_duration_train = trains[0];

    for (int i = 1; i < nconn; i++) {
        if (trains[i].arv - trains[i].dpt > longest_duration_train.arv - longest_duration_train.dpt) {
            longest_duration_train = trains[i];
        }
    }

    return longest_duration_train;
}

struct train* find_trains_from_station(char *station_name) {
    int station_id = city_id(station_name);
    
    static struct train trains_from_station[MAXCONN];
    int count = 0;

    for (int i = 0; i < nconn; i++) {
        if (trains[i].from == station_id) {
            trains_from_station[count++] = trains[i];
        }
    }
    trains_from_station[count].from = -1; // Sentinel
    return trains_from_station;
}

struct train* find_trains_to_station(char *station_name) {
    int station_id = city_id(station_name);
    
    static struct train trains_to_station[MAXCONN];
    int count = 0;

    for (int i = 0; i < nconn; i++) {
        if (trains[i].to == station_id) {
            trains_to_station[count++] = trains[i];
        }
    }
    trains_to_station[count].from = -1; // Sentinel
    return trains_to_station;
}

void print_train_array(struct train *trains) {
    for (int i = 0; trains[i].from != -1; i++) { // Sentinel check
        printf("%s %02d:%02d %s %02d:%02d %d\n",
                city_name[trains[i].from], trains[i].dpt / 60, trains[i].dpt % 60,
                city_name[trains[i].to], trains[i].arv / 60, trains[i].arv % 60,
                trains[i].fare);
    }
}

void cleanup(void) {
    memset(trains, 0, sizeof(trains));
    memset(city_name, 0, sizeof(city_name));
    nconn = 0;
    ncity = 0;
}

int main(void) {
    int count;
    char buf[100];

    int case_id = 0;

    // Read the number of connections
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        sscanf(buf, "%d", &count);

        // Check for end of input
        if (count == 0) {
            break;
        }

        printf("Case %d\n\n", ++case_id);

        process_one_problem(count);

        // Find the most expensive train
        struct train most_expensive_train = find_most_expensive_train();
        printf("Most expensive train: %s %02d:%02d %s %02d:%02d %d\n\n", 
                city_name[most_expensive_train.from], most_expensive_train.dpt / 60, most_expensive_train.dpt % 60,
                city_name[most_expensive_train.to], most_expensive_train.arv / 60, most_expensive_train.arv % 60,
                most_expensive_train.fare);

        // Find the longest duration train
        struct train longest_duration_train = find_longest_duration_train();
        printf("Longest duration train: %s %02d:%02d %s %02d:%02d %d\n\n", 
                city_name[longest_duration_train.from], longest_duration_train.dpt / 60, longest_duration_train.dpt % 60,
                city_name[longest_duration_train.to], longest_duration_train.arv / 60, longest_duration_train.arv % 60,
                longest_duration_train.fare);

        // Find trains from Tokyo
        struct train *trains_from_tokyo = find_trains_from_station("Tokyo");
        printf("Trains from Tokyo:\n");
        print_train_array(trains_from_tokyo);
        printf("\n");

        // Find trains to Tokyo
        struct train *trains_to_tokyo = find_trains_to_station("Tokyo");
        printf("Trains to Tokyo:\n");
        print_train_array(trains_to_tokyo);
        printf("\n");

        cleanup();
    }
    return 0;
}
