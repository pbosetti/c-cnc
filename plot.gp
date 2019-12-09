file = "data.txt"

set ylabel "Position (mm)"
set xlabel "Time (s)"
set grid
plot file using 3:6 with lines title "x",\
 file u 3:9 w l t "xm"
