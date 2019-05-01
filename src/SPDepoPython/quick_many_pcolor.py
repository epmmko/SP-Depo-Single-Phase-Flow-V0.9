# This subroutine plot multiple p-color graph
# with the input of data (no-axis information)
# axis numerical value
# and optional the numerical value of each pattern
#
# Input
# 1) name_no_ext
# 2) start_index
# 3) end_index
# 4) destination_folder_path
# 5) fname_path_r_col
# 6) fname_path_z_col
# 7) title
# 8) x_name
# 9) y_name
# 10) fname_path_t = None (numerical value)
# 11) pattern_format (python format of 10)
#
# All data must be in .csv format
#
# The name_no_ext is the file name
# without any extension
# format of this variable is string
# For example, name_no_ext = ".\\profile_data\\c_tzr_mesh_post__"
# where we have
# .\profile_data\c_tzr_mesh_post__0.csv
# .\profile_data\c_tzr_mesh_post__1.csv
# .\profile_data\c_tzr_mesh_post__2.csv
# ...
# .\profile_data\c_tzr_mesh_post__32.csv
#
# The path in front of the file name must be
# either absolute or relative to the .ipynb/.py file
#
#  data structure
#       r  
#       1     2     3
#  z
#  1  dat11 dat12 dat13
#  2  dat21 dat22 dat23
#  3  dat31 dat32 dat33
#  4  dat41 dat42 dat43
#
#  fname_path_z_col is the file name with path
#  that has only the data of z, with no header
#  the format for this file is
#
#  z1
#  z2
#  ...
#  zn
#
#  fname_path_r_col is the file name with path
#  that has only the data of r (double format)
#  with no header the format for this file is
#
#  r1
#  r2
#  ...
#  rn
#
# fname_path_title fname_path_x_name fname_path_y_name
# are 1 column data file .csv
# for graph title, x-axis title, and y-axis title
#
# fname_path_t is optional, if any
# the number of row must match to the number of pattern
# or the number of file (end_index - start_index + 1)
# When available, this value is to be added right after the title
#
# This program generate pcolor plot and 
# give .jpg output in the same folder 

import sys
import matplotlib.pyplot as plt
import numpy as np
from numpy import genfromtxt

params = sys.argv[1]

def pcolor_multi_fn():
    plt.pcolor(r,z,fn_2d_vec,cmap='jet');
    plt.title(title_text)
    plt.xlabel(output[1])
    plt.ylabel(output[2])
    plt.grid(alpha=0.5)
    plt.colorbar();
    save_name = destination_name + ".jpg"
    plt.savefig(save_name, dpi = 200)
    plt.clf()

    n = fn_2d_vec.shape[0]
    colors = plt.cm.jet(np.linspace(0,1,n))
    for i in range(fn_2d_vec.shape[0]):
        plt.plot(r,fn_2d_vec[i], 'o-', markersize = 1,color = colors[i], linewidth=0.1)
    plt.title("jet color map: variable at " + var_name[2] + " [0] is blue")
    plt.xlabel(output[1])
    plt.ylabel(title_text)
    plt.grid(alpha=0.5)
    save_name = destination_name + "_r" + ".jpg"
    plt.savefig(save_name, dpi = 200)
    plt.clf()

    n = fn_2d_vec.shape[1]
    colors = plt.cm.jet(np.linspace(0,1,n))
    for i in range(fn_2d_vec.shape[1]):
        plt.plot(z,fn_2d_vec[:,i], 'o-', markersize = 1,color = colors[i], linewidth=0.1)
    plt.title("jet color map: variable at " + var_name[1] + " [0] is blue")
    plt.xlabel(output[2])
    plt.ylabel(title_text)
    plt.grid(alpha=0.5)
    save_name = destination_name + "_z" + ".jpg"
    plt.savefig(save_name, dpi = 200)
    plt.clf()

with open(params) as fp:
    command_list = fp.readlines()
command_list = [j.strip('\n') for j in command_list]

name_no_ext = command_list[0]
start_index = int(command_list[1])
end_index = int(command_list[2])
destination_path_name_base = command_list[3]
fname_path_r_col = command_list[4]
fname_path_z_col = command_list[5]
title = command_list[6]
x_name = command_list[7]
y_name = command_list[8]
fname_path_t = command_list[9]

temp_list = [title, x_name, y_name]
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
for i in range(len(var_name)):
    var_name[i] = var_name[i].strip()

title_addition = genfromtxt(fname_path_t, delimiter=',')
r = genfromtxt(fname_path_r_col, delimiter=',')
z = genfromtxt(fname_path_z_col, delimiter=',')

for i in range(start_index,end_index+1):
    name_2d_vec = name_no_ext + str(i) + ".csv"
    fn_2d_vec = genfromtxt(name_2d_vec, delimiter=',')
    title_text = title.format(title_addition[i])
    destination_name = destination_path_name_base + str(i)
    pcolor_multi_fn()