# System import
import os

# Imports for plotting
import sys
import matplotlib
import matplotlib.pyplot as plt

# Run the HO simulation with parameters for successful HO.
print("Run with TTT = 50ms and speed = 20m/s for successful HO.")
os.system('./build/scratch/lena-handover-simple/lena-handover-simple --speed=20 --timeToTrigger=50 --y_for_UE=1200 --inter_eNB_dist=1200')

times1=[]
times2=[]
values1=[]
values2=[]
fd = open('lena-handover-simple-ue-measurements.dat', 'r')
for line in fd:
    l = line.split()
    if line.startswith("#"):
        continue
    if l[2] == "1":
        times1.append(float(l[0]))
        values1.append(float(l[4]))
    elif l[2] == "2":
        times2.append(float(l[0]))
        values2.append(float(l[4]))
fd.close()

if len(times1) == 0:
    print("No data points found, exiting...")
    sys.exit(1)

plt.scatter(times1, values1, marker='.', label='cell 1', color='red')
if len(times2) != 0:
    plt.scatter(times2, values2, marker='.', label='cell 2', color='blue')
plt.xlabel('Time (s)')
plt.ylabel('RSRP (dBm)')
plt.title('RSRP plot for Successful HO')
plt.show(block = False)

# Run the HO simulation with higher speed for RLF.
print("Run with TTT = 50ms and speed = 21m/s for RLF.")
os.system('./build/scratch/lena-handover-simple/lena-handover-simple --speed=21 --timeToTrigger=50 --y_for_UE=1200 --inter_eNB_dist=1200')

times1=[]
times2=[]
values1=[]
values2=[]
fd = open('lena-handover-simple-ue-measurements.dat', 'r')
for line in fd:
    l = line.split()
    if line.startswith("#"):
        continue
    if l[2] == "1":
        times1.append(float(l[0]))
        values1.append(float(l[4]))
    elif l[2] == "2":
        times2.append(float(l[0]))
        values2.append(float(l[4]))
fd.close()

if len(times1) == 0:
    print("No data points found, exiting...")
    sys.exit(1)

plt.scatter(times1, values1, marker='.', label='cell 1', color='red')
if len(times2) != 0:
    plt.scatter(times2, values2, marker='.', label='cell 2', color='blue')
plt.xlabel('Time (s)')
plt.ylabel('RSRP (dBm)')
plt.title('RSRP plot for Successful HO')
plt.show(block = False)
