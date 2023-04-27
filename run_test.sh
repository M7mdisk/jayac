#!/bin/bash

total_time=0

for pop_size in 100 1000 2000
do
    for dimension in 100 1000
    do
        for maxFE in 1000000 10000000
        do
            for method in sphere rosenbrock rastrigin
            do
                echo pop_size=$pop_size dimension=$dimension maxFE=$maxFE method=$method

                output=$(/usr/bin/time -f "time: %e" ./jaya $pop_size $dimension $maxFE $method 2>&1)

                # Extract the time taken from the output and add it to the total
                time_taken=$(echo $output | grep -oP 'time: \K[0-9.]+')
                total_time=$(echo "$total_time + $time_taken" | bc)
                echo $output | grep -oP 'The fit is .*'
            done
        done
    done
done

echo "Total time taken: $total_time seconds"