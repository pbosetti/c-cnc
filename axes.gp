file ="axes.txt"

set xlabel "Time (s)"
set ylabel "position (m)"
set grid
plot file u 1:2 w l t "X",\
  file u 1:3 w l t "Y",\
  file u 1:4 w l t "Z"
