# Gnuplot script for O-16 total cross section
# Log-log plot of cross section vs energy

# Set terminal and output
set terminal png size 1600,1200
set output 'O16_total_cross_section.png'

# Set log-log scale
set logscale xy

# Set labels and title
set title 'O-16 Total Cross Section vs Energy'
set xlabel 'Energy (eV)'
set ylabel 'Cross Section (barns)'

# Set grid
set grid

# Set axis ranges (optional - let gnuplot auto-scale)
# set xrange [1e-12:1e7]
# set yrange [1e-3:1e4]

# Plot the data
plot 'O16_MT1.dat' using 1:2 with lines title 'Total Cross Section' lc rgb 'blue' lw 2

# Alternative: plot with error bars if needed
# plot 'O16_MT1.dat' using 1:2:3 with errorbars title 'Total Cross Section'

# Set legend
set key top right

# Refresh the plot
replot

# Print completion message
print "Plot saved as O16_total_cross_section.png"
