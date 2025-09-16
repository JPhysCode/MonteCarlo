set datafile separator "\t"
DATAFILE = "pi_estimates.txt"
OUTFILE = "pi_hist.png"
TITLE = "Pi estimates histogram - Quarter-circle"

# Compute stats on first column, skipping header row
stats DATAFILE using 1 every ::1 name 'A' nooutput

# Bin settings
numBins = 30
binwidth = (A_max > A_min) ? (A_max - A_min)/numBins : 0.0001
bin(x,width) = width * floor(x/width) + width/2.0

# Output
set term pngcairo size 800,600
set output OUTFILE
set title TITLE
set xlabel "pi estimate"
set ylabel "count"
set boxwidth binwidth absolute
set style fill solid 0.5
set grid ytics

# Plot histogram (skip header using every ::1)
plot DATAFILE using (bin($1,binwidth)):(1.0) every ::1 smooth freq with boxes lc rgb "#ff7f0e" notitle
