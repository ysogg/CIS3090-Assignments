threads = [2, 4, 6, 8, 10, 12]
tasks = [2, 4, 6, 8, 10, 12] 
# all iterNums
# 500000000
# 1000000000 one billion
# 1500000000
# 2000000000
# 2500000000
# 3000000000

# Example parallelTiming file
# Iterations evenly split across 8 tasks, 8 threads for each
# 8
# 62500000
# 62500000
# 62500000
# 62500000
# 62500000
# 62500000
# 62500000
# 62500000

# Parallel Timings
fNum = 1
iterNum = 500000000
for x in threads:
    f = open("ParallelTiming{}.txt".format(fNum), "w+")
    f.write(str(4)+"\n")

    for j in range (4):
        f.write(str(int(iterNum/4))+"\n")
    iterNum += 500000000
    f.close()
    fNum += 1

# Task Timings
iterNum = 2000000000
fNum = 1
for x in tasks:
    f = open("TaskTiming{}.txt".format(fNum), "w+")
    f.write("4\n")

    for j in range(x):
        f.write(str(int(iterNum/x))+"\n")
    f.close()
    fNum+=1

#Thread Timings
iterNum = 2000000000
fNum = 1
for x in tasks:
    f = open("ThreadTiming{}.txt".format(fNum), "w+")
    f.write(str(x)+"\n")

    for j in range(x):
        f.write(str(int(iterNum/x))+"\n")
    f.close()
    fNum+=1
