import sys

MAXCITY = 100
MAXCONN = 2001
INFINITE = 99999999

# Global Variables
city_name = []
ncity = 0
trains = []
nconn = 0

# Helper Functions
def city_id(name):
    global city_name, ncity
    if name in city_name:
        return city_name.index(name)
    else:
        city_name.append(name)
        ncity += 1
        return ncity - 1

def parse_connection(buf):
    global trains, nconn
    from_station, dpt_hm, to_station, arv_hm, fare = buf.split()

    dpt_hour, dpt_min = dpt_hm.split(":")
    arv_hour, arv_min = arv_hm.split(":")

    dpt = int(dpt_hour) * 60 + int(dpt_min)
    arv = int(arv_hour) * 60 + int(arv_min)
    fare = int(fare)

    train = {
        "from": city_id(from_station),
        "to": city_id(to_station),
        "dpt": dpt,
        "arv": arv,
        "fare": fare
    }
    trains.append(train)
    nconn += 1

def find_most_expensive_train():
    return max(trains, key=lambda train: train["fare"])

def find_longest_duration_train():
    return max(trains, key=lambda train: train["arv"] - train["dpt"])

def find_trains_from_station(station_name):
    station_id = city_id(station_name)
    return [train for train in trains if train["from"] == station_id]

def find_trains_to_station(station_name):
    station_id = city_id(station_name)
    return [train for train in trains if train["to"] == station_id]

def print_train_array(trains):
    for train in trains:
        from_name = city_name[train["from"]]
        to_name = city_name[train["to"]]
        dpt_hour, dpt_min = divmod(train["dpt"], 60)
        arv_hour, arv_min = divmod(train["arv"], 60)
        fare = train["fare"]
        print(f"{from_name} {dpt_hour:02d}:{dpt_min:02d} {to_name} {arv_hour:02d}:{arv_min:02d} {fare}")

def cleanup():
    global trains, city_name, nconn, ncity
    trains = []
    city_name = []
    nconn = 0
    ncity = 0

def process_one_problem(count):
    global trains
    for _ in range(count):
        buf = input().strip()
        parse_connection(buf)

def main():
    case_id = 0
    while True:
        try:
            buf = input().strip()
            count = int(buf)
            if count == 0:
                break

            print(f"Case {case_id + 1}\n")
            case_id += 1

            process_one_problem(count)

            # Find the most expensive train
            expensive_train = find_most_expensive_train()
            print("Most expensive train:")
            print_train_array([expensive_train])
            print()

            # Find the longest duration train
            longest_duration_train = find_longest_duration_train()
            print("Longest duration train:")
            print_train_array([longest_duration_train])
            print()

            # Find all trains from Tokyo
            print("Trains from Tokyo:")
            trains_from_Tokyo = find_trains_from_station("Tokyo")
            print_train_array(trains_from_Tokyo)
            print()

            # Find all trains to Tokyo
            print("Trains to Tokyo:")
            trains_to_Tokyo = find_trains_to_station("Tokyo")
            print_train_array(trains_to_Tokyo)
            print()

            cleanup()

        except EOFError:
            break

if __name__ == "__main__":
    main()
