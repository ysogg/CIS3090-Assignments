# Cases considered
# 1.1 # threads 1/2 # cores | # tasks < # threads
# 1.2 # threads 1/2 # cores | # tasks = # threads
# 1.3 # threads 1/2 # cores | # tasks > # threads

# 2.1 # threads = # cores | # tasks < # threads
# 2.2 # threads = # cores | # tasks = # threads
# 2.3 # threads = # cores | # tasks > # threads

# 3.1 # thread 2x # cores | # tasks < # threads
# 3.2 # thread 2x # cores | # tasks = # threads
# 3.3 # thread 2x # cores | # tasks > # threads

threads = [2, 4, 8] 
# half of threads, equal to threads, double num threads
tasks = [1, 2, 4, 2, 4, 8, 4, 8, 16] 
# all iterNums
# 500000000
# 600000000
# 700000000
# 800000000
# 900000000 *nine hundred milly
# 1000000000 *one billy

# One of these per iterNum
# --
# Loop for each case considered * num iterNums
line = 1
threadPos = 0
# x=1
curr = 500000000

for x in range(1, 55):
    # for i in range(9):
    f = open("case{}.txt".format(x), "w+")
    f.write(str(threads[threadPos])+"\n")

    # gets number of lines to write out
    for j in range(tasks[line-1]):
        # iteration number divided by line count
        f.write(str(int(curr/tasks[line-1]))+"\n")
    f.close()

    # if threadPos == 2 and line == 9:
    #     threadPos = 0 
    if line == 3 or line == 6:
        threadPos += 1
    if line == 9:
        line = 1   
        threadPos = 0
    else:
        line += 1
    if x == 9 or x == 18 or x == 27 or x == 36 or x == 45:
        curr += 100000000
