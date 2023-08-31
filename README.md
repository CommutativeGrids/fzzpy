# FastZigzag Python Wrapper

This repository provides a Python wrapper for the [Fast Computation of Zigzag Persistence](https://github.com/taohou01/fzz), originally implemented in C++. The bindings allow users to easily utilize the power of the FastZigzag C++ library directly within Python.

## Getting Started

### Prerequisites

- CMake for building
- Boost library >= 1.65.1
- PHAT library >=1.4, <=1.5. It's included as a submodule, but you can change the directory path to your own if desired.

### Installation

First, clone the repository:

```bash
git clone --recursive <REPO_URL>
```

Note: The `--recursive` flag ensures that submodules (like PHAT) are also cloned.

Then, navigate to the directory and install the Python package:

```bash 
pip install .
```
## Usage

### Within Python

Input:
- `filt_simp`: A list of tuples. Each tuple at index $k$ representing a simplex to be inserted or deleted in the $k$-th step
- `filt_op`: A list of booleans. Each boolean at index $k$ representing whether the simplex at index $k$ in `filt_simp` is inserted or deleted in the $k$-th step

```python
from fzzpy import compute

# Define your filtration simplicities and filtration operations
filt_simp = [[0], [1], [2], [0, 1], [0, 2], [1, 2], [0, 1, 2], [0, 1, 2], [1, 2], [0, 1]]
filt_op = [True, True, True, True, True, True, True, False, False, False]

# Compute persistence
result = compute(filt_simp, filt_op)

# Print the result
print(result)
# [(0, 2, 3), (0, 3, 4), (1, 6, 6), (0, 1, 10), (0, 10, 10), (1, 8, 8)]
```

### From a Filtration File

Can read in a filtration file in the format as specified in `fzz`.

With `sample_filt.txt`:
```txt
i 0
i 1
i 2
i 0 1
i 0 2
i 1 2
i 0 1 2
d 0 1 2
d 1 2
d 0 1
```

```python
from fzzpy import compute, parse_filtration_file, write_persistence_intervals

# Read the filtration file
filt_simp, filt_op = parse_filtration_file("filtration.txt")

# Compute persistence
result = compute(filt_simp, filt_op)

# Write the results to another file
write_persistence_intervals(result, "output_intervals.txt")
```
This then generate a file following the format specified in `fzz`.


## License

TBD