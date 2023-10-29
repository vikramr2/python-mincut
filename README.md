# python-mincut

Python wrapper for [VieCut](https://github.com/VieCut/VieCut)

- [python-mincut](#python-mincut)
  - [Requirements](#requirements)
    - [UIUC EngrIT Systems](#uiuc-engrit-systems)
  - [Compilation](#compilation)
    - [via `pip` install](#via-pip-install)
    - [via Source Code](#via-source-code)
  - [Usage](#usage)
    - [Basic Usage](#basic-usage)
    - [Balanced VieCut-Cactus Edge Case](#balanced-viecut-cactus-edge-case)
   
## Requirements

- `python3.9` or higher
- `cmake 3.2.0` or higher
- `openmpi` and `gcc` of any version

### UIUC EngrIT Systems

- These instructions are specific for users on an EngrIT cluster (such as Valhalla or the Campus Cluster) under the University of Illinois at Urbana-Champaign
- You can get all the needed packages to run the pipeline via the following commands

```bash
module load python3/3.10.0
module load cmake/3.25.1
module load gcc
module load openmpi
```

## Compilation

### via `pip` install

Simply run

```bash
pip install git+https://github.com/vikramr2/python-mincut
```

### via Source Code

Clone the repository and run the following commands from the root of this project

```bash
mkdir build
cd build
cmake .. && make
cd ..
```

## Usage

Import the `PyGraph` object and compute mincut as a member function

### Basic Usage

If you are using options that aren't `cactus` algorithm and `balanced=True` for mincut, you can do the following:

```Python
from pymincut.pygraph import PyGraph

nodes = # Arbitrary node list of any data type (e.g. [1, 2, 3], [1, 'a', 0.5], etc.)
edges = # Edges are lists of 2-tuples (e.g. [(1, 2), (1, 3)], [(1, 'a'), (0.5, 1)], etc.). The elements in the tuples must be in the node list.

G = PyGraph(nodes, edges)

''' Mincut Arguments

algorithm (str)            : name of algorithm used to compute mincut (e.g. noi, vc, cactus)
queue_implementation (str) : priority queue implementation (e.g. bqueue, bstack, heap)
balanced (bool)            : is the mincut balanced?
'''
light_partition, heavy_partition, cut_size = G.mincut("noi", "bqueue", False)
```

### Balanced VieCut-Cactus Edge Case

If you would like balanced mincuts, it is suggested that you instead do the following:

```Python
from pymincut.pygraph import PyGraph

nodes = [1, 2, 3, 4, 5]
edges = [(1, 2), (1, 3), (4, 5)]

G = PyGraph(nodes, edges)

mincut_result = G.mincut("cactus", "bqueue", True)

'''
In the event that your graph has more than 
two connected components, pymincut will return
more than two partitions
'''
partitions = mincut_result[:-1]
cut_size = mincut_result[-1]
```

If your options are set to `algorithm='cactus'` and `balanced=True`, your graph actually has a mincut of 0 (i.e. it is disconnected), and it has more than two connected components, `pymincut` wont just return a light and heavy partition. It will actually return **ALL** connected components of the graph along with a cut size of 0.
  
You can retrieve the mincut result information as done in the code above.
