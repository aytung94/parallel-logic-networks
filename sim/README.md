# How to Simulate

1. Make sure you are in the `sim/` directory and moved your <Design>.h file into this folder.

2. Run the following command to compile simulator.

```bash
$ make Design.h=<YourDesignName>.h cuda
```
3. Create input.txt to test design.

File should contain the number of simulation passes on the first line. 
The inputs should be specified on the following lines. 
Example for 4 input design with 2 passes: 
2
1 0 1 0
0 0 0 1

3. Run simulator.

If NVIDIA GPU is native, then run ./simulator. 
If TACC super computer is used, load module using 'module load cuda', then 'sbatch RunSim'.

4. Read ouput.txt file. 

The ouputs from the designs will be specified here, with each line being the ouput corresponding to input.txt line.
Console will display run time and final pass of simulator.  


