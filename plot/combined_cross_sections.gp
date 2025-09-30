# Gnuplot script for combined cross sections (O-16 and H-1)
# Log-log plot of cross section vs energy

# Set terminal and output
set terminal png size 1600,1200
set output 'combined_cross_sections.png'

# Set log-log scale
set logscale xy

# Set labels and title
set title 'Total Cross Sections: O-16 and H-1 vs Energy'
set xlabel 'Energy (eV)'
set ylabel 'Cross Section (barns)'

# Set grid
set grid

# Set legend
set key top right

# Plot both curves
plot 'O16_MT1.dat' using 1:2 with lines title 'O-16 Total Cross Section' lc rgb 'blue' lw 2, \
     'H1_MT1.dat' using 1:2 with lines title 'H-1 Total Cross Section' lc rgb 'red' lw 2

# Refresh the plot
replot

# Print completion message
print "Combined plot saved as combined_cross_sections.png"
