#create 2 series or less in case the second file name does not exist
# suggested values are given below
#
# file_xy1 = "delta_mm_average.csv"
# file_xy2 = "data_delta_hr_mm.csv"
# file_output = "sim_exp_delta_ave.jpg"
# title = "average_deposit_thickness"
# x_axis = "time_[hr]"
# y_axis = "thickness_[mm]"
# plotmode1 = "o--"
# msize1 = 4
# lwidth1 = 0.2
# fill1 = "none"
# medge1 = 0.8
# label1 = "graph1"
# plotmode2 = "s"
# msize2 = 2
# lwidth2 = 0.1
# fill2 = "full"
# medge2 = 1
# label2 = "graph2"
# grid_alpha = 0.5
# dpi = 200
# xmin = 0.0
# ymin = 0.0
#
#

import matplotlib.pyplot as plt
import sys
from numpy import genfromtxt

file_xy1 = sys.argv[1]
file_xy2 = sys.argv[2]
file_output = sys.argv[3]
title = sys.argv[4]
x_axis = sys.argv[5]
y_axis = sys.argv[6]
plotmode1 = sys.argv[7]
msize1 = sys.argv[8]
lwidth1 = sys.argv[9]
fill1 = sys.argv[10]
medge1 = sys.argv[11]
label1 = sys.argv[12]
plotmode2 = sys.argv[13]
msize2 = sys.argv[14]
lwidth2 = sys.argv[15]
fill2 = sys.argv[16]
medge2 = sys.argv[17]
label2 = sys.argv[18]
grid_alpha = sys.argv[19]
dpi = sys.argv[20]

temp_list = [title, x_axis, y_axis]
str_output = []
for item in temp_list:
    str_output.append(item.replace("_", " "))
    
try:
    xy1 = genfromtxt(file_xy1, delimiter=',')
except:
    pass
try:
    xy2 = genfromtxt(file_xy2, delimiter=',')
except:
    pass

plt.figure()
try:
    plt.plot(xy1[:,0], xy1[:,1], plotmode1, fillstyle = fill1,
             markeredgewidth = float(medge1), markersize = float(msize1),
             linewidth = float(lwidth1), label = label1)
except:
    pass
try:
    plt.plot(xy2[:,0], xy2[:,1], plotmode2, fillstyle = fill2,
             markeredgewidth = float(medge2), markersize = float(msize2),
             linewidth = float(lwidth2), label = label2)
except:
    pass
plt.title(str_output[0])
plt.xlabel(str_output[1])
plt.ylabel(str_output[2])
plt.grid(alpha = float(grid_alpha))
lg = plt.legend(bbox_to_anchor=(1,1))
plt.savefig(file_output, dpi = float(dpi), bbox_extra_artists=(lg,), bbox_inches='tight')