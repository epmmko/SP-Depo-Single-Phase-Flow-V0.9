import matplotlib.pyplot as plt
import sys
import numpy as np
from numpy import genfromtxt


# https://bit.ly/2owuUVu (tutorial on get argv from sys)
# This script call matplotlib with
#  plt.pcolor(r,z,tzr);
#  tzr has z row, and r column
#  tzr.shape = (z_size, r_size)
#  r.size = (Nr,)
#  z.size = (Nz,)
#
# argv1 = 2D csv file that contain the T(r,z) value
#   e.g. tzr.csv
# argv2 = 2D csv file that contain the r value
#   e.g. tzr_r.csv
# argv3 = 2D csv file that contain the Z value
#   e.g. tzr.csv
# argv4 = title (value and unit of T)
# argv5 = x-axis name
# argv6 = y-axis name

name_tzr = sys.argv[1]
name_r = sys.argv[2]
name_z = sys.argv[3]

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

tzr = genfromtxt(name_tzr, delimiter=',')
r = genfromtxt(name_r, delimiter=',')
z = genfromtxt(name_z, delimiter=',')

plt.figure()
plt.pcolor(r,z,tzr,cmap='jet');
plt.title(output[0])
plt.xlabel(output[1])
plt.ylabel(output[2])
plt.grid(alpha=0.5)
plt.colorbar();
plt.savefig(name_tzr[:-3] + "jpg", dpi = 200)

plt.figure()
n = tzr.shape[0]
colors = plt.cm.jet(np.linspace(0,1,n))
for i in range(tzr.shape[0]):
    plt.plot(r,tzr[i], 'o-', markersize = 1,color = colors[i], linewidth=0.1)
plt.title("jet color map: variable at " + var_name[2] + " [0] is blue")
plt.xlabel(output[1])
plt.ylabel(output[0])
plt.grid(alpha=0.5)
plt.savefig(name_tzr[:-4] + "_r" + ".jpg", dpi = 200)

plt.figure()
n = tzr.shape[1]
colors = plt.cm.jet(np.linspace(0,1,n))
for i in range(tzr.shape[1]):
    plt.plot(z,tzr[:,i], 'o-', markersize = 1,color = colors[i], linewidth=0.1)
plt.title("jet color map: variable at " + var_name[1] + " [0] is blue")
plt.xlabel(output[2])
plt.ylabel(output[0])
plt.grid(alpha=0.5)
plt.savefig(name_tzr[:-4] + "_z" + ".jpg", dpi = 200)

