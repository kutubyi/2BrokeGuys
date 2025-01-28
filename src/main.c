#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

#define INFINITE 99999999

// cost tables
int from_station1[MAXCITY][MAXCONN],
    from_station2[MAXCITY][MAXCONN], 
    to_station1[MAXCITY][MAXCONN],
    to_station2[MAXCITY][MAXCONN];

#define BIAS (24*60)

struct result {
    int cost;       
    int station;     
    int start_time;  
    int end_time;    
} result;

char station1[18], station2[18];
int start, end, meeting;

int cmp_arv(const void *t1, const void *t2);
void prepare_data(void);
int change(struct train tv[], int p, int st, int dpttime);
void make_table(int v[MAXCITY][MAXCONN], int org, struct train tv[]);
struct result calc_cost(int city);
struct result solve(void);

int min(int a, int b);

void parse_connection(char *buf);
int city_id(char *name);

// functions for checkpoint 2
struct train find_most_expensive_train(void);
struct train find_longest_duration_train(void);
struct train* find_trains_from_station(char *station_name);
struct train* find_trains_to_station(char *station_name);
void print_train_array(struct train *trains);

void filter_connections_in_time_range(int start_time, int end_time);

int cmp_arv(const void *t1, const void *t2) {
    return ((struct train*)t1)->arv - ((struct train*)t2)->arv;
}

void prepare_data(void) {
    int i;

    for(i=0; i<nconn; i++) {
        rtrains[i].from = trains[i].to;
        rtrains[i].to = trains[i].from;
        rtrains[i].dpt = BIAS - trains[i].arv;
        rtrains[i].arv = BIAS - trains[i].dpt;
        rtrains[i].fare = trains[i].fare;
    }
    qsort(trains, nconn, sizeof(struct train), cmp_arv);
    qsort(rtrains, nconn, sizeof(struct train), cmp_arv);
}

int change(struct train tv[], int p, int st, int dpttime) {
    while (p >= 0) {
        if((tv[p].to == st) && (tv[p].arv <= dpttime)) break;
        p--;
    }
    return p;
}

void make_table(int v[MAXCITY][MAXCONN], int org, struct train tv[]) {
    int ti, i, a;

    for(i = 0; i < ncity; i++) {
        v[i][0] = INFINITE;
    }
    v[org][0] = 0;

    for(ti = 0; ti < nconn; ti++) {
        for(i = 0; i < ncity; i++) {
            v[i][ti+1] = v[i][ti];
        }
        a = change(tv, ti-1, tv[ti].from, tv[ti].dpt);
        v[tv[ti].to][ti+1] = min(v[tv[ti].to][ti], 
                                tv[ti].fare + v[tv[ti].from][a+1]);
    }
}

struct result calc_cost(int city) {
    int a = 0, d = nconn-1;
    int stay;
    int c, min_c = INFINITE;
    int min_city, min_start, min_end;

    while(1) {
        stay = (BIAS - rtrains[d].arv) - trains[a].arv;
        if(stay < meeting) {
            d --;
            if(d < 0) break;
            continue;
        }
        c = from_station1[city][a+1] + from_station2[city][a+1] +
            to_station1[city][d+1] + to_station2[city][d+1];
        if(c < min_c) {
            min_c = c;
            min_city = city;
            min_start = trains[a].arv;
            min_end = BIAS - rtrains[d].arv;
        }
        a ++;
        if(a >= nconn) break;
    }
    struct result res = {min_c, min_city, min_start, min_end};
    return res;
}

struct result solve(void) {
    int c;
    int cost, min_cost; 
    struct result curr_result;
    struct result min_result = {INFINITE, -1, -1, -1};

    prepare_data();

    make_table(from_station1, city_id(station1), trains);
    make_table(from_station2, city_id(station2), trains);
    make_table(to_station1, city_id(station1), rtrains);
    make_table(to_station2, city_id(station2), rtrains);

    for(c = 0; c < ncity; c++) {
        curr_result = calc_cost(c);
        if(curr_result.cost < min_result.cost) {
            min_result = curr_result;
        }
    }

    if(min_result.cost >= INFINITE) {
        min_result.cost = 0;
    }
    return min_result;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

void parse_connection(char *buf) {
    char from[18], to[18];
    int dpt[2], arv[2], fare;

    sscanf(buf, "%s %d:%d %s %d:%d %d", from, &(dpt[0]), &(dpt[1]), to, &(arv[0]), (&arv[1]), &fare);

    trains[nconn].from = city_id(from);
    trains[nconn].to = city_id(to);
    trains[nconn].dpt = dpt[0]*60+dpt[1];
    trains[nconn].arv = arv[0]*60+arv[1];
    trains[nconn].fare = fare;
    nconn++;
}

int city_id(char *name) {
    int i;

    for(i = 0; i < ncity; i++) {
        if (strcmp(name, &(city_name[i][0])) == 0) return i;
    }

    strcpy(&(city_name[ncity][0]), name);
    return ncity++;
}

struct train find_most_expensive_train(void) {
    struct train most_expensive_train = trains[0];

    for (int i = 1; i < nconn; i++) {
        if (trains[i].fare > most_expensive_train.fare) {
            most_expensive_train = trains[i];
        }
    }

    return most_expensive_train;
}

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
    trains_from_station[count].from = -1; // sentinel
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
    trains_to_station[count].from = -1; // sentinel
    return trains_to_station;
}

void print_train_array(struct train *trains) {
    for (int i = 0; trains[i].from != -1; i++) { // sentinel check
        printf("%s %02d:%02d %s %02d:%02d %d\n",
                city_name[trains[i].from], trains[i].dpt / 60, trains[i].dpt % 60,
                city_name[trains[i].to], trains[i].arv / 60, trains[i].arv % 60,
                trains[i].fare);
    }
}

void filter_connections_in_time_range(int start_time, int end_time) {
    struct train filtered_trains[MAXCONN];
    int filtered_count = 0;

    for (int i = 0; i < nconn; i++) {
        if (trains[i].dpt >= start_time && trains[i].arv <= end_time) {
            filtered_trains[filtered_count++] = trains[i];
        }
    }
    memcpy(trains, filtered_trains, filtered_count * sizeof(struct train));
    nconn = filtered_count;
}

void cleanup(void) {
    nconn = 0;
    ncity = 0;

    memset(trains, 0, sizeof(trains));
    memset(rtrains, 0, sizeof(rtrains));

    memset(from_station1, 0, sizeof(from_station1));
    memset(from_station2, 0, sizeof(from_station2));
    memset(to_station1, 0, sizeof(to_station1));
    memset(to_station2, 0, sizeof(to_station2));
}

int main(int argc, char *argv[]) {
    int count;
    char buf[64];

    int dpt[2], arv[2];

    if (argc != 6) {
        fprintf(stderr, "Usage: %s <駅1> <駅2> <出発時刻> <帰着時刻> <面会時間> < <input_file>\n", argv[0]);
        return 1;
    }

    strcpy(station1, argv[1]);
    strcpy(station2, argv[2]);
    sscanf(argv[3], "%d:%d", &dpt[0], &dpt[1]);
    start = dpt[0] * 60 + dpt[1];
    sscanf(argv[4], "%d:%d", &arv[0], &arv[1]);
    end = arv[0] * 60 + arv[1];
    meeting = atoi(argv[5]);

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
		sscanf(buf, "%d", &count);

		if(count == 0) break;

		for(int i = 0; i < count; i++) {
			if(fgets(buf, sizeof(buf), stdin) == NULL) break;
			parse_connection(buf);
        }
        // Apply time filtering (e.g., 8:00 AM to 6:00 PM)
        filter_connections_in_time_range(start, end);

        struct result result = solve();
        if (result.cost != 0) {
            printf("%d %s: %02d:%02d - %02d:%02d\n",
                   result.cost,
                   city_name[result.station],
                   result.start_time / 60, result.start_time % 60,
                   result.end_time / 60, result.end_time % 60);
        }
        else {
            printf("0\n");
        }

        cleanup();
    }
}
