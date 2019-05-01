import matplotlib.pyplot as plt
import sys
import numpy as np
from numpy import genfromtxt

#Create 1 series plot with pyplot
# https://bit.ly/2owuUVu (tutorial on get argv from sys)
# This script call matplotlib with
#  plt.plot(x,y, "o", markersize = 2);
#  x.size = (N,)
#  y.size = (N,)
#
# argv1 = 2 column csv file that contain the xy value
#   e.g. try_plot_xy.csv
# argv2 = output picture file name
#   e.g. big_equation_plot.jpg
# argv3 = title (value and unit)
# argv4 = x-axis name
# argv5 = y-axis name

file_xy = sys.argv[1]
file_output = sys.argv[2]
title = sys.argv[3]
x_axis = sys.argv[4]
y_axis = sys.argv[5]

temp_list = [title, x_axis, y_axis]
output = []
var_name = []
for item in temp_list:
    idx = item.find('[')
    if(idx != -1):
        output.append(item[:idx] + " " + item[idx:])
        var_name.append(item[:idx])
    else:
        output.append(item)
        var_name.append(item)

xy = genfromtxt(file_xy, delimiter=',')

plt.figure()
plt.plot(xy[:,0],xy[:,1], "o-", markersize = 2, linewidth=0.1)
plt.title(output[0])
plt.xlabel(output[1])
plt.ylabel(output[2])
plt.grid(alpha=0.5)
plt.savefig(file_output, dpi = 200)