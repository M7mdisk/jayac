for pop_size in 100 1000 2000
do
    for dimension in 100 1000
    do
        for maxFE in 1000000 10000000
        do
            for method in sphere rosenbrock rastrigin
            do
                echo pop_size=$pop_size dimension=$dimension maxFE=$maxFE method=$method
                time ./jaya $pop_size $dimension $maxFE $method
                echo ---
            done
        done
    done
done
