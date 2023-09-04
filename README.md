# Python Wrapper for Fast Computation of Zigzag Persistence

This repository provides a Python wrapper for [Fast Computation of Zigzag Persistence](https://github.com/taohou01/fzz), originally implemented in C++. The bindings here allow users to easily utilize the power of the C++ library directly within Python.

## Getting Started

### Prerequisites

- [CMake](https://cmake.org/): >= 3.5
- [Boost](https://www.boost.org/): >= 1.5
- [PHAT](https://bitbucket.org/phat-code/phat/src/master/): between 1.4 and 1.5. It is included as a submodule in `\libs\phat`, but you can change the directory path in `CMakeLists.txt` to your own.
- [OpenMP](https://www.openmp.org/): Version 5.0 or higher (201811 or higher).. This is required for parallelization.
- [pybind11]

### Installation

Clone this repository:

```bash
git clone --recursive https://github.com/CommutativeGrids/fzzpy.git
```

Note: The `--recursive` flag ensures that submodules (like PHAT) are also cloned.

Then navigate to the directory and install the Python package, the installation configuration is specified in `pyproject.toml`:

```bash 
pip install .
```

#### For Mac Users

If you encounter issues with OpenMP not being found even after installation, it might be because Apple's default Clang does not come with OpenMP support. In such cases, you can use the LLVM version of Clang provided by Homebrew which includes OpenMP.

1. First, ensure we have the LLVM package installed:
   ```bash
   brew install llvm
   ```
2. After installing LLVM via Homebrew, set the CC and CXX environment variables to point to the Clang binaries provided by LLVM:
   ```bash
   export CC=/usr/local/opt/llvm/bin/clang
   export CXX=/usr/local/opt/llvm/bin/clang++
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


## 
In `fzz.cpp`, the following line
```c++
phat::compute_persistence_pairs< phat::twist_reduction >( pairs, bound_chains );
```
is replaced by
```c++
phat::compute_persistence_pairs< phat::chunk_reduction >( pairs, bound_chains );
```
to utilize multiple CPU cores computation supported by `PHAT`.

## License

TBD