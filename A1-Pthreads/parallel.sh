echo iterations,timems > parallelTimings.csv

for i in {1..7..1}
do
    for j in {1..35..1}
    do
        S=$(date +%s%N)
        ./A1 ParallelTiming$i.txt csv >> parallelTimings.csv
        E=$(date +%s%N)
        echo $(( ($E- $S) / 1000000 )) >> parallelTimings.csv
        # printf "ParallelTiming$i.txt\n" | ./A1 csv | awk '{sub(/Enter filename: /,""); print }' >> parallelTimings.csv
    done
done

echo iterations,timems > threadTimings.csv

for i in {1..7..1}
do
    for j in {1..35..1}
    do
        S=$(date +%s%N)
        ./A1 ThreadTiming$i.txt csv >> threadTimings.csv
        E=$(date +%s%N)
        echo $(( ($E- $S) / 1000000 )) >> threadTimings.csv
        # printf "ThreadTiming$i.txt\n" | ./A1 csv | awk '{sub(/Enter filename: /,""); print }' >> threadTimings.csv
    done
    printf ",,\n" >> threadTimings.csv
done

echo iterations,timems > taskTimings.csv

for i in {1..7..1}
do
    for j in {1..35..1}
    do
        S=$(date +%s%N)
        ./A1 TaskTiming$i.txt csv >> taskTimings.csv
        E=$(date +%s%N)
        echo $(( ($E- $S) / 1000000 )) >> taskTimings.csv
        # printf "TaskTiming$i.txt\n" | ./A1 csv | awk '{sub(/Enter filename: /,""); print }' >> taskTimings.csv
    done
    printf ",,\n" >> taskTimings.csv
done