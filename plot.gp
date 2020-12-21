# see http://hirophysics.com/gnuplot/gnuplot08.html for Gnuplot help
fig="data.txt"
set yrange [0:120]
set grid
set title "PID step response"
plot fig using 2:3 with lines title "X" linetype rgb "red", \
  fig u 2:4 w l t "Y" lt rgb "green", \
  fig u 2:5 w l t "Z" lt rgb "blue"