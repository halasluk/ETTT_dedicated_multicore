# ETTT_dedicated_multicore
Implementation of algorithms for the combination of ET and TT scheduling on dedicated resources with precedences.

For further information see (dspace TBD).

## Dependencies

*  A modern C++ compiler supporting the **C++20 standard** (recent versions of `clang` or `g++`).
*  The **CMake** build system.
*  `numpy` and `matplotlib` libraries for **Python 3**.

## Build Instructions

To compile the project and build it, use the following commands:

        cmake -DCMAKE_BUILD_TYPE=Release CMakeLists.txt
        make

This yields a binary **ETTT_dedicated_multicore**.

## How To Use

### Instance Format

The instances are stored in `.dat` files. Each instance contains a header on the first line. The header for ET schedulability instances contains:

*  **Number of ET tasks**, **Number of processors**.

The header for the combination of ET and TT tasks instances contains:

*  **Number of ET tasks**, **Number of TT tasks**, **Number of processors**.

Each following line then corresponds to either ET or TT task. The ET task's parameters are stored in the following order:

* **Task ID**: ID of the ET task,
* **Period**: period of the ET task,
* **Deadline**: deadline of the ET task,
* **Earliest release time**: lower bound on the release time,
* **Latest release time**: upper bound on the release time,
* **Shortest execution time**: lower bound on the execution time,
* **Longest execution time**: upper bound on the execution time,
* **Priority**: priority of the ET task,
* **Length of the sequence of processors**: number of processors to which the ET task is dedicated,
* **Sequence of processors**: processor indexes from the sequence of processors.

The TT task's parameters are stored in the following order:

* **Task ID**: ID of the TT task,
* **Period**: period of the TT task,
* **Deadline**: deadline of the TT task,
* **Release time**: release time of the TT task,
* **Execution time**: execution time of the TT task,
* **Length of the sequence of processors**: number of processors to which the TT task is dedicated,
* **Sequence of processors**: processor indexes from the sequence of processors.

### Run Instructions

To run the pessimism measurement experiment for the SAG approach to schedulability analysis, use the following command: `./ETTT_dedicated_multicore ET_C ./instances/et/pes/0.dat`. The program outputs the results into `stdout`.

To run the pessimism measurement experiment for either FGG or FGG-EB algorithm, use the following command: `./ETTT_dedicated_multicore ETTT_C ./instances/ettt/pes/0.dat <algorithm>`, where `<algorithm>` can be substituted by either `FGG` or `FGG-EB`. The program outputs the results into `stdout`.

To run the evaluation of the SAG approach to schedulability analysis for a ceratin instance from a certain dataset, use the following command: `./ETTT_dedicated_multicore ET_P ./instances/et/eval/<dataset>.dat <save_path> <instance>`, where `<dataset>` can be substituted by any number from `[0-8]`, and `<instance>` can be substituted by any number from `[0-999]`. The result is saved in `<save_path>/<instance>.dat` file. The result contains one line with:

* **Number of ET tasks**, **Number of ET jobs**, **Schedulability result**, **Runtime**.

To run the evaluation of either the FGG algorithm or the FGG-EB algorithm for a ceratin instance from a certain dataset, use the following command: `./ETTT_dedicated_multicore ETTT_P ./instances/ettt/eval/<dataset>.dat <save_path> <instance> <algorithm>`, where `<dataset>` can be substituted by any number from `[0-8]`, `<instance>` can be substituted by any number from `[0-999]`, and `<algorithm>` can be substituted by either `FGG` or `FGG-EB`. The result is saved in `<save_path>/<instance>.dat` file. The result contains one line with:

* **Number of TT tasks**, **Number of TT jobs**, **Schedulability result**, **Runtime**, **Number of ET tasks**, **Number of ET jobs**.

To generate instances for ET schedulability analysis, use the following command: `./ETTT_dedicated_multicore ET_G <save_path> <num_verts>`, where `<num_verts>` shall be substituted by the desired number of vertices in the network model. The generated datasets are stored in directory given by `<save_path>`. Other parameters for the instance generation can be changed manually in the function `generateEtInstances` in file `./src/instances/instanceGenerator.cpp`.

To generate instances for the combination of ET and TT tasks, use the following command: `./ETTT_dedicated_multicore ETTT_G <save_path> <num_verts>`, where `<num_verts>` shall be substituted by the desired number of vertices in the network model. The generated datasets are stored in directory given by `<save_path>`. Other parameters for the instance generation can be changed manually in the function `generateEtttInstances` in file `./src/instances/instanceGenerator.cpp`.

### Results and Visualization

The results of the evaluation of the SAG approach are stored in `./instances/et/eval/results/`, the results of the evaluation of the FGG algorithm are stored in `./instances/ettt/eval/results_FGG/`, and the results of the evaluation of the FGG-EB algorithm are stored in `./instances/ettt/eval/results_FGG-EB/`. To visualize the evaluation results, use the following command: `python3 ./instances/performance_visualization.py ./instances/et/eval/results/ ./instances/ettt/eval/<algorithm_results>/`, where `<algorithm_results>` can be substituted by `results_FGG` or `results_FGG-EB` for the results of FGG or FGG-EB, respectively.
