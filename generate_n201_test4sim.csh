
# May need to hack dhrystone/dhry_1.c number of runs from 16000 to 2, and hack the too small times
# May need to hack coremark/core_main.c number of runs and hack the too small times
make dasm  SIMTEST=1 PROGRAM=demo_pic4sim BOARD=nuclei-n200 CORE=n201   DOWNLOAD=iecm
make dasm  SIMTEST=1 PROGRAM=dhrystone4sim BOARD=nuclei-n200 CORE=n201   DOWNLOAD=iecm
make dasm  SIMTEST=1 PROGRAM=dhrystone4sim_best BOARD=nuclei-n200 CORE=n201   DOWNLOAD=iecm
make dasm  SIMTEST=1 PROGRAM=coremark4sim  BOARD=nuclei-n200 CORE=n201   DOWNLOAD=iecm
make dasm  PROGRAM=FreeRTOSv9.0.0 BOARD=nuclei-n200 CORE=n201   DOWNLOAD=iecm NANO_PFLOAT=0
rm fpga_n201_test4sim -rf
mkdir fpga_n201_test4sim
cp software/sim_tests/demo_pic4sim fpga_n201_test4sim/ -rf
cp software/sim_tests/dhrystone4sim fpga_n201_test4sim/ -rf
cp software/sim_tests/dhrystone4sim_best fpga_n201_test4sim/ -rf
cp software/sim_tests/coremark4sim  fpga_n201_test4sim/ -rf
cp software/FreeRTOSv9.0.0 fpga_n201_test4sim/ -rf
tar -czvf fpga_n201_test4sim.tar.gz fpga_n201_test4sim
