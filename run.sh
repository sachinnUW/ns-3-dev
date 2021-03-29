./waf --run "lena-handover-many --simTime=10 -timeToTrigger=40 -hysteresis=1" > results/HO_40_1.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=40 -hysteresis=3" > results/HO_40_3.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=40 -hysteresis=5" > results/HO_40_5.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=80 -hysteresis=1" > results/HO_80_1.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=80 -hysteresis=3" > results/HO_80_3.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=80 -hysteresis=5" > results/HO_80_5.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=256 -hysteresis=1" > results/HO_256_1.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=256 -hysteresis=3" > results/HO_256_3.txt
./waf --run "lena-handover-many --simTime=10 -timeToTrigger=256 -hysteresis=5" > results/HO_256_5.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=40 -hysteresis=1" > results/HO_40_1_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=40 -hysteresis=3" > results/HO_40_3_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=40 -hysteresis=5" > results/HO_40_5_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=80 -hysteresis=1" > results/HO_80_1_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=80 -hysteresis=3" > results/HO_80_3_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=80 -hysteresis=5" > results/HO_80_5_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=256 -hysteresis=1" > results/HO_256_1_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=256 -hysteresis=3" > results/HO_256_3_0.txt
./waf --run "lena-handover-many --useUdp=0 --simTime=10 -timeToTrigger=256 -hysteresis=5" > results/HO_256_5_0.txt
