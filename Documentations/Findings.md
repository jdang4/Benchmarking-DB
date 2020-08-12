# Findings Found From Benchmark Application

Based on the collected results, it seems that at a low thread count (somewhere in the range of using 10 threads), Redis is consistently performing significantly better than PostgreSQL. The numbers for Redis are much faster than that of PostgreSQL. However, once the thread counts start exceeding a count of 50, the numbers between Redis and PostgreSQL are quite similar to each other. The time differences between the two are quite close to one another.

While Redis does seem to prove its more effective than PostgreSQL for this kind of application, it is not enough to conclude that Redis is the most effective database to use. I would not exactly say that comparing these two databases was a fair commparision to make considering that Redis falls under NoSQL and PostgreSQL being classified as a relational database. In order to make a better comparision, especially against Redis, I think it would be a good idea to add other open-source cloud databases that are similar to that of Redis. 

For visuals of the collected results, please go to the "Graphs" directory under the "Documentations" directory.