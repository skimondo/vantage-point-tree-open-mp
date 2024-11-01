
```sh
./build/Desktop-Release/bin/bench_vptree 
cmake -G Ninja -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
srun -c 8 ./build-release/bin/bench_vptree 
scp skimondo@inf5171.calculquebec.cloud:execution_time_vs_granularity.png ~/Documents

```

```sh
module load python
pip install pandas
pip install matplotlib
```