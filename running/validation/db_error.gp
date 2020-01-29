set terminal postscript eps color "Helvetica" 24
set output 'db_pred_error.eps'
set key bottom right font "Helvetica, 20"

set xlabel "Prediction Error (sec)"
set ylabel "CDF"

N_all=system("wc -l error.txt")
N_near=system("wc -l error_near.txt")
set xr [0.01:10]
set logscale x
plot 'error.txt' u ($2):($1/N_all) w l title "All Prediction", 'error_near.txt' u ($2):($1/N_near) w l title "Near Prediction"

set output 'db_warn.eps'
N_warn=system("wc -l warning.txt")
set xr [0:8]
set xlabel "T_h (sec)"
unset logscale x
plot 'warning.txt' u ($2):($1/N_warn) w l title "offset"
