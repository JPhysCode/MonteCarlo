# Quarter-circle π Estimator

This app estimates π by sampling points uniformly in the unit square [0,1)×[0,1) and counting how many fall inside the quarter of a unit circle (x² + y² ≤ 1). The estimate is π ≈ 4·inside/samples.

## Inputs (super simple)

Inputs are read from a plain text file `input.txt` in this folder.
The file contains exactly two numbers on two lines:

1. Number of samples (unsigned integer)
2. RNG seed (unsigned integer)

### Example `input.txt`
```
100000
42
```

## Running
From the build directory after building, run:
```
./quartercircle
```
The program will look for `input.txt` in the same folder as the executable.
