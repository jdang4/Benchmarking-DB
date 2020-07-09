from csv import reader
import matplotlib.pyplot as plt 
import sys

def oneKeyCommand(redis, postgres, num, title) :
    redis_data = redis[num]
    postgres_data = postgres[num]
    x = []
    redis_y = []
    postgres_y = []

    max_val = 0

    for nums in redis_data :
        x.append(int(nums[0]))
        redis_y.append(float(nums[1]))

    for val in redis_y :
        if val > max_val :
            max_val = val 

    for nums in postgres_data :
        postgres_y.append(float(nums[1]))
    
    for val in postgres_y :
        if val > max_val :
            max_val = val 


    plt.plot(x, redis_y, 'r-', label = "redis")

    plt.plot(x, postgres_y, 'g-', label = "postgreSQL")
    
    plt.ylim(0, max_val)

    plt.title('Redis vs PostgreSQL %s' % title)

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


if __name__ == "__main__" :

    redis = getDataFromCSV("stats/redis-stats.csv")
    postgres = getDataFromCSV("stats/postgres-stats.csv")
     
    oneKeyCommand(redis, postgres, 0, 'Read 1 Key')
    oneKeyCommand(redis, postgres, 1, 'Insert 1 Key')
    oneKeyCommand(redis, postgres, 2, 'Update 1 Key')
    oneKeyCommand(redis, postgres, 3, 'Delete 1 Key')

    oneKeyCommand(redis, postgres, 13, 'Simultaneous Readers')
    oneKeyCommand(redis, postgres, 14, 'Simultaneous Tasks')
    oneKeyCommand(redis, postgres, 15, 'Simultaneous Transactions')

    
        
