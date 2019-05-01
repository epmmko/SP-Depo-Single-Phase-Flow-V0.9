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
# argv1 = 1 column csv file that contain the x value
#   e.g. try_plot_x.csv
# argv2 = 1 column csv file that contain the y value
#   e.g. try_plot_y.csv
# argv3 = output picture file name
#   e.g. big_equation_plot.jpg
# argv4 = title (value and unit)
# argv5 = x-axis name
# argv6 = y-axis name

file_x = sys.argv[1]
file_y = sys.argv[2]
file_output = sys.argv[3]
title = sys.argv[4]
x_axis = sys.argv[5]
y_axis = sys.argv[6]

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

x = genfromtxt(file_x, delimiter=',')
y = genfromtxt(file_y, delimiter=',')

plt.figure()
plt.plot(x,y, "o", markersize = 2)
plt.title(output[0])
plt.xlabel(output[1])
plt.ylabel(output[2])
plt.savefig(file_output, dpi = 200)