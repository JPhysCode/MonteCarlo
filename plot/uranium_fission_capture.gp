# Gnuplot script for U-235 and U-238 fission + capture cross sections
# Log-log plot of combined cross section vs energy

# Set terminal and output
set terminal png size 1600,1200
set output 'uranium_fission_capture.png'

# Set log-log scale
set logscale xy

# Set labels and title
set title 'U-235 and U-238: Fission + Capture Cross Sections vs Energy'
set xlabel 'Energy (eV)'
set ylabel 'Cross Section (barns)'

# Set grid
set grid

# Set legend
set key top right

# Plot both curves
plot 'U235_fission_capture.dat' using 1:2 with lines title 'U-235 Fission + Capture' lc rgb 'blue' lw 2, \
     'U238_fission_capture.dat' using 1:2 with lines title 'U-238 Fission + Capture' lc rgb 'red' lw 2

# Refresh the plot
replot

# Print completion message
print "Uranium fission + capture plot saved as uranium_fission_capture.png"
