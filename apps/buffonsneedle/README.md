# Buffon's Needle π Estimator

This app estimates π using Buffon's Needle problem - a classic Monte Carlo method.

## Algorithm

1. **Setup**: Parallel lines spaced `lineSpacing` units apart
2. **Drop needle**: Random position and angle
3. **Check hit**: Does needle cross a line?
4. **Count hits**: Track how many needles cross lines
5. **Estimate π**: Use Buffon's formula

## Buffon's Formula

π ≈ (2 × needleLength × numSamples) / (lineSpacing × hits)

**Why it works:**
- Probability of hit = (2 × needleLength) / (π × lineSpacing)
- Rearranging: π = (2 × needleLength) / (lineSpacing × probability)
- Probability ≈ hits / numSamples

## Inputs

Inputs are read from `input.txt` in this folder (4 numbers, one per line):

1. **numSamples**: Number of needle drops (unsigned integer)
2. **seed**: RNG seed for reproducibility (unsigned integer)  
3. **needleLength**: Length of needle (double)
4. **lineSpacing**: Distance between parallel lines (double)

### Example `input.txt`
```
100000
42
1.0
2.0
```

## Running

From the build directory after building, run:
```
./buffonsneedle
```

The program will look for `input.txt` in the same folder as the executable.

## Output

Shows: samples, seed, needle length, line spacing, number of hits, and π estimate.

Example: `samples=100000 seed=42 needle=1.0 spacing=2.0 hits=31831 pi≈3.14159`
