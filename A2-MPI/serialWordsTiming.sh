echo words,timems > serialWordsTimes.csv

for i in {1..8..1}
do
    for j in {1..31..1}
    do
        S=$(date +%s%N)
        ./a2decrypt_serial mult$i.txt american-english
        E=$(date +%s%N)
        echo $i,$(( ($E- $S) / 1000000 )) >> serialWordsTimes.csv
    done
done