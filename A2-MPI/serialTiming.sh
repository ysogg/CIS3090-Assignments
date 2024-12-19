echo chars,timems > serialTimes.csv

for i in {3..10..1}
do
    for j in {1..31..1}
    do
        S=$(date +%s%N)
        ./serialDecrypt cipher$i.txt american-english
        E=$(date +%s%N)
        echo $i,$(( ($E- $S) / 1000000 )) >> serialTimes.csv
    done
done