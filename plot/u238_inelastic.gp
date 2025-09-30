# Gnuplot script for U-238 inelastic cross section
# Log-log plot of inelastic cross section vs energy

# Set terminal and output
set terminal png size 1600,1200
set output 'u238_inelastic.png'

# Set log-log scale
set logscale xy

# Set labels and title
set title 'U-238 Total Inelastic Cross Section vs Energy'
set xlabel 'Energy (eV)'
set ylabel 'Cross Section (barns)'

# Set grid
set grid

# Set legend
set key top right

# Plot the curve
plot 'U238_inelastic.dat' using 1:2 with lines title 'U-238 Inelastic Cross Section' lc rgb 'red' lw 2

# Refresh the plot
replot

# Print completion message
print "U-238 inelastic cross section plot saved as u238_inelastic.png"
