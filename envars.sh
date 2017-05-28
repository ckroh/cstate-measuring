#set cpu affinity pinning threads to specific cpus
export GOMP_CPU_AFFINITY=0,1,12
export GOMP_STACKSIZE=32M
#specifiy number of program threads to be launched
export OMP_NUM_THREADS=3

#ensure that Score-P uses tsc as timer, same as used by cpuidle-measure records
export SCOREP_TIMER='tsc'
export SCOREP_TOTAL_MEMORY=3900M
#only create Score-P traces
export SCOREP_ENABLE_PROFILING=false
export SCOREP_ENABLE_TRACING=true
#attach Score-P cpuidle plugin to environment
export SCOREP_METRIC_PLUGINS=cpuidle_plugin,$SCOREP_METRIC_PLUGINS
export LD_LIBRARY_PATH=/home/s1428123/Beleg/scorep_plugin_cpuidle:$LD_LIBRARY_PATH
export LIBRARY_PATH=/home/s1428123/Beleg/scorep_plugin_cpuidle:$LIBRARY_PATH
#specify which cpus should be observed by the cpuidle plugin
export SCOREP_METRIC_CPUIDLE_PLUGIN=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23
#ni-plugin settings
export SCOREP_METRIC_NIPLUGIN_SYNC=diana/s0-fast/watts
export SCOREP_METRIC_NIPLUGIN_SYNC_CPUS=0,1,2,3,4,5,6,7,8,9,10,11
export SCOREP_METRIC_NIPLUGIN=diana/s1-fast/watts,diana/s0-fast/watts

#define cache sizes
export CACHE_FULL=32000
export CACHE_3Q=24000
export CACHE_HALF=16000
export CACHE_1Q=8000
export CACHE_EMPTY=0
#random number to prevent compiler optimizations of the trigger program
export RAND_NUM=23457
