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

#define INFINITE 99999999

int from_hakodate[MAXCITY][MAXCONN],
    from_tokyo[MAXCITY][MAXCONN], 
    to_hakodate[MAXCITY][MAXCONN],
    to_tokyo[MAXCITY][MAXCONN];

#define BIAS (24*60)

int cmp_arv(struct train *t1, struct train *t2);
void prepare_data(void);
int change(struct train tv[], int p, int st, int dpttime);
void make_table(int v[MAXCITY][MAXCONN], int org, struct train tv[]);
int calc_cost(int city);
int solve(void);

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

int cmp_arv(struct train *t1, struct train *t2) {
    return (t1->arv - t2->arv);
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

int calc_cost(int city) {
    int a = 0, d = nconn-1;
    int stay;
    int c, min_c = INFINITE;

    while(1) {
        stay = (BIAS - rtrains[d].arv) - trains[a].arv;
        if(stay < 30) {
            d --;
            if(d < 0) break;
            continue;
        }
        c = from_hakodate[city][a+1] + from_tokyo[city][a+1] +
            to_hakodate[city][d+1] + to_tokyo[city][d+1];
        if(c < min_c)
            min_c = c;
        a ++;
        if(a >= nconn) break;
    }
    return min_c;
}

int solve(void) {
    int c;
    int cost, min_cost; 

    prepare_data();

    make_table(from_hakodate, city_id("Hakodate"), trains);
    make_table(from_tokyo, city_id("Tokyo"), trains);
    make_table(to_hakodate, city_id("Hakodate"), rtrains);
    make_table(to_tokyo, city_id("Tokyo"), rtrains);

    min_cost = INFINITE;
    for(c = 0; c < ncity; c++) {
        cost = calc_cost(c);
        if(cost < min_cost)
            min_cost = cost;
    }

    if(min_cost >= INFINITE)
        min_cost = 0;

    return min_cost;
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

    memset(from_hakodate, 0, sizeof(from_hakodate));
    memset(from_tokyo, 0, sizeof(from_tokyo));
    memset(to_hakodate, 0, sizeof(to_hakodate));
    memset(to_tokyo, 0, sizeof(to_tokyo));
}

int main(void) {
    int count;
    char buf[64];

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
		sscanf(buf, "%d", &count);

		if(count == 0) break;

		for(int i = 0; i < count; i++) {
			if(fgets(buf, sizeof(buf), stdin) == NULL) break;
			parse_connection(buf);
        }
        // Apply time filtering (e.g., 8:00 AM to 6:00 PM)
        filter_connections_in_time_range(8 * 60, 18 * 60);

        int result = solve();
        printf("%d\n", result);

        cleanup();
    }
}