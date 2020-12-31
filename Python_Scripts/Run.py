# System import
import os

# Run the HO simulation with parameters for successful HO.
print("Run with TTT = 50ms and speed = 20m/s for successful HO.")
os.system('./build/scratch/lena-handover-simple/lena-handover-simple --speed=20 --timeToTrigger=50 --y_for_UE=1200 --inter_eNB_dist=1200')

# Run the HO simulation with higher speed for RLF.
print("Run with TTT = 50ms and speed = 21m/s for successful HO.")
os.system('./build/scratch/lena-handover-simple/lena-handover-simple --speed=21 --timeToTrigger=50 --y_for_UE=1200 --inter_eNB_dist=1200')



