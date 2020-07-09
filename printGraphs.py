from csv import reader
import matplotlib.pyplot as plt 


def oneKeyCommand(m, num, title) :
    data = m[num]
    x = []
    y = []

    for nums in data :
        x.append(int(nums[0]))
        y.append(float(nums[1]))

    plt.plot(x, y)

    plt.xlabel('Trials')
    plt.ylabel('Elapsed Time (sec)')

    plt.title(title)

    plt.show()


def getUpperLimit(avg) :
    str_time = str('%f' % (avg))

    num_after_decimal = str_time.split('.')[1]

    index = 0
    for i in range(0, len(num_after_decimal)) :
        if num_after_decimal[i] == '0' :
            index += 1

        else :
            index += 1
            break 

    limit = '0.'

    if index == 2 :
        limit = '0.5'

    else :
        for i in range(1, (index - 2)) :
            limit += '0'

        limit += '1'

    return limit


def simultaneousCommands(m, num, title, command) :
    data = m[num]
    simult = []
    times = []
    averages = []

    for nums in data :
        simult.append(int(nums[0]))
        times.append(float(nums[1]))
        averages.append(float(nums[2]))

    avg_command = sum(averages) / len(averages)

    limit = getUpperLimit(avg_command)
    
    fig, ax1 = plt.subplots()

    ax2 = ax1.twinx()
    ax1.plot(simult, times, 'b-')
    ax2.plot(simult, averages, 'r-')
    ax2.set_ylim([0, float(limit)])

    plt.title(title)
    ax1.set_xlabel('Number of Simultaneous')
    ax1.set_ylabel('Elapsed Time (sec)', color='b')
    ax2.set_ylabel('Average Time per %s' % (command), color='r')

    plt.show()


if __name__ == "__main__" :

    myMap = {}

    myMap[0] = []

    with open('stats/postgres-stats.csv', 'r') as read_obj :
        csv_reader = reader(read_obj)

        header = next(csv_reader)

        count = 0

        index = 0

        limit = 5

        for row in csv_reader :
            if index > 12 :
                limit = 10
            
            if count < limit :
                myMap[index].append(row)
                #print(row)

            elif count == limit + 2 :
                count = -1
                index += 1
                myMap[index] = []
                #print()

            count += 1


     
    oneKeyCommand(myMap, 0, 'Read 1 Key')
    oneKeyCommand(myMap, 1, 'Insert 1 Key')
    oneKeyCommand(myMap, 2, 'Update 1 Key')
    oneKeyCommand(myMap, 3, 'Delete 1 Key')
    simultaneousCommands(myMap, 13, 'Simultaneous Readers', 'Read')
    simultaneousCommands(myMap, 14, 'Simultaneous Tasks', 'Task')
    simultaneousCommands(myMap, 15, 'Simultaneous Transactions', 'Transaction')
    
        
