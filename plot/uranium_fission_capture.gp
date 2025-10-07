# Gnuplot script for U-235 and U-238 fission and capture cross sections
# Log-log plot of separate fission and capture cross sections vs energy

# Set terminal and output
set terminal png size 2560,1920
set output 'uranium_fission_capture.png'

# Set log-log scale
set logscale xy

# Set labels and title
set title 'U-235 and U-238: Fission and Capture Cross Sections vs Energy'
set xlabel 'Energy (MeV)'
set ylabel 'Cross Section (barns)'

# Set grid
set grid

# Set legend
set key top right

# Plot four curves from single table (convert eV to MeV)
# Blue shades for U235, red shades for U238
plot 'uranium_fission_capture.dat' using ($1/1e6):2 with lines title 'U-235 Fission' lc rgb 'dark-blue' lw 2, \
     'uranium_fission_capture.dat' using ($1/1e6):3 with lines title 'U-235 Capture' lc rgb 'light-blue' lw 2, \
     'uranium_fission_capture.dat' using ($1/1e6):4 with lines title 'U-238 Fission' lc rgb 'dark-red' lw 2, \
     'uranium_fission_capture.dat' using ($1/1e6):5 with lines title 'U-238 Capture' lc rgb 'light-red' lw 2

# Refresh the plot
replot

# Print completion message
print "Uranium fission and capture plot saved as uranium_fission_capture.png"
