echo words,timems > parallelWordsTimes.csv

for i in {1..8..1}
do
    for j in {1..31..1}
    do
        S=$(date +%s%N)
        mpiexec -n 4 ./a2decrypt mult$i.txt american-english
        E=$(date +%s%N)
        echo $i,$(( ($E- $S) / 1000000 )) >> parallelWordsTimes.csv
    done
done