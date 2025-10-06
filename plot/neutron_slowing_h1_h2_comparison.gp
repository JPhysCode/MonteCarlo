# Gnuplot script for H1 vs H2 neutron slowing down comparison
# Energy vs collision number with log scale on y-axis

set terminal png size 1200, 800
set output 'neutron_slowing_h1_h2_comparison.png'

# Set up the plot
set title 'Neutron Slowing Down Comparison\nH1 (Protium) vs H2 (Deuterium)' font ",16"
set xlabel 'Collision Number' font ",12"
set ylabel 'Neutron Energy (MeV)' font ",12"

# Use log scale for y-axis
set logscale y

# Format y-axis to show proper scientific notation
set format y "%.2e"

# Set axis ranges - let both axes auto-fit to data
set autoscale x
set autoscale y

# Add grid for better readability
set grid

# Define colors and styles
set style line 1 lc rgb "blue" lw 2
set style line 2 lc rgb "red" lw 2

# Plot both datasets (convert eV to MeV by dividing by 1e6)
plot 'neutron_slowing_h1.dat' using 1:($2/1e6) with lines linestyle 1 title 'H1 (Protium)', \
     'neutron_slowing_h2.dat' using 1:($2/1e6) with lines linestyle 2 title 'H2 (Deuterium)'

# Add some reference lines for context (will auto-scale to data range)
set arrow from graph 0, 2.5e-5 to graph 1, 2.5e-5 nohead linecolor rgb "gray" linetype 2
set label "Thermal Energy (0.025 eV)" at graph 0.5, 5e-5 font ",10"

set arrow from graph 0, 0.001 to graph 1, 0.001 nohead linecolor rgb "gray" linetype 2
set label "Epithermal Region (1 keV)" at graph 0.5, 0.002 font ",10"

set arrow from graph 0, 1 to graph 1, 1 nohead linecolor rgb "gray" linetype 2
set label "Fast Neutron Region (1 MeV)" at graph 0.5, 1.5 font ",10"

# Show the plot
replot

print "Comparison plot saved as: neutron_slowing_h1_h2_comparison.png"
