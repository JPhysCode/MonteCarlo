# Gnuplot script for combined cross sections (O-16 and H-1)
# Log-log plot of cross section vs energy

# Set terminal and output
set terminal png size 2560,1920
set output 'combined_cross_sections.png'

# Set log-log scale
set logscale xy

# Set labels and title
set title 'Total Cross Sections: O-16 and H-1 vs Energy'
set xlabel 'Energy (MeV)'
set ylabel 'Cross Section (barns)'

# Set grid
set grid

# Set legend
set key top right

# Plot both curves from single table (convert eV to MeV)
plot 'H1_O16_total.dat' using ($1/1e6):2 with lines title 'H-1 Total Cross Section' lc rgb 'red' lw 2, \
     'H1_O16_total.dat' using ($1/1e6):3 with lines title 'O-16 Total Cross Section' lc rgb 'blue' lw 2

# Refresh the plot
replot

# Print completion message
print "Combined plot saved as combined_cross_sections.png"
