from csv import reader
import matplotlib.pyplot as plt 

def printReadsOne(m) :
    data = m[0]
    x = []
    y = []

    for nums in data :
        x.append(int(nums[0]))
        y.append(float(nums[1]))

    plt.plot(x, y)

    plt.xlabel('Trials')
    plt.ylabel('Elapsed Time (sec)')

    plt.title('Reading 1 Key')

    print('showing graph')
    plt.show()


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
                #print(row)

            elif count == 7 :
                count = -1
                index += 1
                myMap[index] = []
                #print()

            count += 1


    #print("\n\n")

    printReadsOne(myMap)
                

        
