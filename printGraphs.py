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
        y.append(float(nums[3]))

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

        limit = 3

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


def combineMaps(m1, m500, m5K, m500K) :
    myMap = {}

    allMaps = [m1, m500, m5K, m500K]

    for m in allMaps :
        for key in m.keys() :
            if key not in myMap :
                myMap[key] = []

            myMap[key].extend(m[key])


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

    #plt.xlim(0, 410)

    plt.title(title)
    plt.xlabel('Running Time (min)')
    plt.ylabel('Average Elapsed Time (sec)')

    plt.show()


if __name__ == "__main__" :
    
    redis_1 = getDataFromCSV("stats/1/redis-stats.csv")
    #postgres_1= getDataFromCSV("stats/1/postgres-stats.csv") 
    
    redis_500 = getDataFromCSV("stats/500/redis-stats.csv") 
    #postgres_500 = getDataFromCSV("stats/500/postgres-stats.csv")  

    redis_5K = getDataFromCSV("stats/5000/redis-stats.csv")  
    #postgres_5K = getDataFromCSV("stats/5000/postgres-stats.csv")  

    redis_500K = getDataFromCSV("stats/500000/redis-stats.csv")  
    #postgres_500K = getDataFromCSV("stats/500000/postgres-stats.csv")  

    #redis_1M = getDataFromCSV("stats/1000000/redis-stats.csv")   
    #postgres_1M = getDataFromCSV("stats/1000000/postgres-stats.csv")  

    running_redis = getRunningDataFromCSV("stats/500000/redis-running-stats.csv")

    running_postgres = getRunningDataFromCSV("stats/500000/postgres-running-stats.csv")

    getRunningStatsGraph(running_redis, 16, "Redis 10 Simultaneous Transactions")

    getRunningStatsGraph(running_postgres, 16, "PostgreSQL 10 Simultaneous Transactions")





        
