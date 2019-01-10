
# May need to hack dhrystone/dhry_1.c number of runs from 16000 to 2, and hack the too small times
# May need to hack coremark/core_main.c number of runs and hack the too small times
make dasm  SIMTEST=1 PROGRAM=demo_eclic4sim BOARD=nuclei-n22 ISA=IMAC   DOWNLOAD=ilm
make dasm  SIMTEST=1 PROGRAM=dhrystone4sim BOARD=nuclei-n22 ISA=IMAC   DOWNLOAD=ilm
#make dasm  SIMTEST=1 PROGRAM=dhrystone4sim_best BOARD=nuclei-n22 ISA=IMAC   DOWNLOAD=ilm
make dasm  SIMTEST=1 PROGRAM=coremark4sim  BOARD=nuclei-n22 ISA=IMAC   DOWNLOAD=ilm
#make dasm  PROGRAM=FreeRTOSv9.0.0 BOARD=nuclei-n22 ISA=IMAC   DOWNLOAD=ilm NANO_PFLOAT=0
rm fpga_nuclei_test4sim -rf
mkdir fpga_nuclei_test4sim
cp software/sim_tests/demo_eclic4sim fpga_nuclei_test4sim/ -rf
cp software/sim_tests/dhrystone4sim fpga_nuclei_test4sim/ -rf
#cp software/sim_tests/dhrystone4sim_best fpga_nuclei_test4sim/ -rf
cp software/sim_tests/coremark4sim  fpga_nuclei_test4sim/ -rf
#cp software/FreeRTOSv9.0.0 fpga_nuclei_test4sim/ -rf
tar -czvf fpga_nuclei_test4sim.tar.gz fpga_nuclei_test4sim
