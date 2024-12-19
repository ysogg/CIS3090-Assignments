echo iterations,pi,time > serialTimings.csv
for N in {1..35..1}
do
    ./serial 500000000 >> serialTimings.csv
done

for N in {1..35..1}
do
    ./serial 1000000000 >> serialTimings.csv
done

for N in {1..35..1}
do
    ./serial 1500000000 >> serialTimings.csv
done

for N in {1..35..1}
do
    ./serial 2000000000 >> serialTimings.csv
done

for N in {1..35..1}
do
    ./serial 2500000000 >> serialTimings.csv
done

for N in {1..35..1}
do
    ./serial 3000000000 >> serialTimings.csv
done
