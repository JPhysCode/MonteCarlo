# Gnuplot script for water and uranium macroscopic cross sections
# Log-log plot comparing macroscopic cross sections vs energy

# Set terminal and output
set terminal png size 1600,1200
set output 'macroscopic_cross_sections.png'

# Set log-log scale
set logscale xy

# Set labels and title
set title 'Macroscopic Cross Sections: Water (H2O) vs Natural Uranium'
set xlabel 'Energy (eV)'
set ylabel 'Macroscopic Cross Section (cm⁻¹)'

# Set grid
set grid

# Set legend
set key top right

# Plot both curves
plot 'water_macroscopic.dat' using 1:2 with lines title 'Water (H2O) at 4°C' lc rgb 'blue' lw 2, \
     'uranium_macroscopic.dat' using 1:2 with lines title 'Natural Uranium (0.72% U235, 99.28% U238)' lc rgb 'red' lw 2

# Refresh the plot
replot

# Print completion message
print "Combined macroscopic cross sections plot saved as macroscopic_cross_sections.png"
