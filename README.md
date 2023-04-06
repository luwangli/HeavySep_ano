# HeavySep
## Intro
HeavySep enhance exiting sketches by filtering elephant flow efficiently and accuracy.
We apply HeavySep to two typical sketches (HS-CM and HS-CU) on two common stream processing tasks 
(Flow frequency, and HeavyHitter ).
## File
- HS3/. the implementation of HeavySep-CM and HeavySep-CU
- CMSketch/. the implementation of CM
- CUSketch/. the implementation of CU
- ColdFilter/. the implementation of ColdFilter
- HeavyGuard/. the implementation of HeavyGuard
- ASketch/. the implementation of ASketch
- elasticsketch/. the implementation of ElsaticSketch
- data/. a test case.

## Requirement
Ensure g++ and make are installed. Our experimental platform is equipped with Ubuntu 16, g++ 5.4 and make 4.1

## Compile and Run
There are three stream processing task in this repo.
### Flow Frequency Task
note in this task, we compare HS-CM, HS-CU, CM, CU, ColdFilter and ASketch.
```
$cd Task_FlowFre
$make clean
$make
$./main,out
```
Then input parameter according to the guideline. You will get the result of CM, CU, HS-CU, HS-CM, ColdFilter,
ASketch and ElasticSketch.


### HeavyHitter Task
note in HeavyHitter Task, we compare CMHeap, CUHeap, HeavySep, HeavyGuardian, WavingSketch.

```markdown
$cd Task_HH
$make clean
$make
$./main.out
```

Note that you can change the configuration of HeavySep, e.g. number of rows and buckets in the example source code for testing.


