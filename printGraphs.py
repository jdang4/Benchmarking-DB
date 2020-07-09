from csv import reader

if __name__ == "__main__" :

    myMap = {}

    index = 0

    myMap[index] = []

    with open('stats/redis-stats.csv', 'r') as read_obj :
        csv_reader = reader(read_obj)

        header = next(csv_reader)

        count = 0

        header = 0

        for row in csv_reader :
            if count < 5 :
                if header == 0 :
                    myMap[index].append(row)
                print(row)

            elif count == 7 :
                count = -1
                index += 1
                myMap[index] = []
                print()

            count += 1


    print("\n\n")

    for read in myMap[12] :
        print(read)
                

        
