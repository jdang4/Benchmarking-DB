from csv import reader
import matplotlib.pyplot as plt 
import sys
import pandas as pd

def getCommandGraph(db, num, title) :
    data = db[num]
    x = []
    y = []

    for nums in data :
        x.append(int(nums[0]))
        y.append(float(nums[1]))

    plt.plot(x, y)

    plt.title(title)

    plt.xlabel('Trials')
    plt.ylabel('Elapsed Time (sec)')

    plt.legend()
    plt.show()


def getDataFromCSV(file) :
    myMap = {}

    myMap[0] = []

    with open(file, 'r') as read_obj :
        csv_reader = reader(read_obj)

        header = next(csv_reader)

        count, index = 0, 0

        limit = 5

        for row in csv_reader :
            if index > 12 :
                limit = 10
            
            if count < limit :
                myMap[index].append(row)

            elif count == limit + 2 :
                count = -1
                index += 1
                myMap[index] = []

            count += 1
    
    return myMap 


def getRunningDataFromCSV(filename) :
    df = pd.read_csv(filename)
    
    myMap = {}
    index = 0

    for column in df.columns :
        myMap[index] = df[column].values.tolist()
        index += 1

    #print(myMap[1])
    return myMap


def getRunningStatsGraph(db, num, title) :
    timeStamp = db[0]

    values = db[num]

    plt.plot(timeStamp, values)

    plt.xlim(0, 1200)

    plt.title(title)
    plt.xlabel('Running Time (sec)')
    plt.ylabel('Average Elapsed Time (sec)')

    plt.show()


if __name__ == "__main__" :

    redis = getDataFromCSV("stats/redis-stats.csv")
    postgres = getDataFromCSV("stats/postgres-stats.csv")

    #redis = getRunningDataFromCSV("stats/redis-running-stats.csv")
    #postgres = getRunningDataFromCSV("stats/postgres-running-stats.csv")

    #getRunningStatsGraph(redis, 1, 'Reading 1 Key') 

    getCommandGraph(redis, 0, 'Redis Read')
    getCommandGraph(postgres, 0, 'PostgreSQL Read')

    getCommandGraph(redis, 1, 'Redis Insert')
    getCommandGraph(postgres, 1, 'PostgreSQL Insert')

    getCommandGraph(redis, 2, 'Redis Update')
    getCommandGraph(postgres, 2, 'PostgreSQL Update')

    getCommandGraph(redis, 3, 'Redis Delete')
    getCommandGraph(postgres, 3, 'PostgreSQL Delete')


    """
    oneKeyCommand(redis, postgres, 0, 'Read 1 Key')
    oneKeyCommand(redis, postgres, 1, 'Insert 1 Key')
    oneKeyCommand(redis, postgres, 2, 'Update 1 Key')
    oneKeyCommand(redis, postgres, 3, 'Delete 1 Key')

    oneKeyCommand(redis, postgres, 13, 'Simultaneous Readers')
    oneKeyCommand(redis, postgres, 14, 'Simultaneous Tasks')
    oneKeyCommand(redis, postgres, 15, 'Simultaneous Transactions')
    """
    
        
