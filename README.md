=============================
Program format and structure:
=============================
1. The assembler:

    * with two data structures to store respectively labels and mnemonic table with expected operands.
    * uses a single routine for both passes.
    * detects label errs.
    * consistent and sensible formatting with sensible program structure. 
    * sensible variable, function & type names with explanatory comments.
		* advanced listing file.
		* assembles test programs.
	  * cannot implements and demonstrates the instruction SET.

2. The emulator:

	* with three data structures to store respectively memory, mnemonic table with expected operands.
    * loads object file, 
    * by giving the instructions like "-before/-after" , the program can produce memory dump before/after execution.
    * by giving the instruction like "-trace", the program prints the result of program executed.


=============================
Testing:
=============================

1. The assembler:

I have tested  4 examples shown as bellow. The errors only occurred in the file test2.asm and 
the program immediately stopped on the second pass. 

The others were tested without any errors detected and produced three output files, listing file, 
error log file and machine readable object file, for each example. 

After comparing the listing files and object file of memcpy.asm , memcpy.l, triangle.l memcpy.o 
with the implemented result given on the assignment, both of them show no difference 
from those provided( if there is different  that is fine too) 
 

1)
#Input: asm test1.asm
#Output: 
a. test1.l 
b. test1.log (without any error messages)
c. test1.o

2)
#Input:asm test2.asm
#Output: 
a. test2.l 
b. test2.log (with error messages shown as below but also in log file)
c. test2.o

error messages:
Error @Line:4   Duplicate label definition
Error @Line:5   "nonesuch" is not yet defined
Error @Line:6   Badly formatted number - 08ge
Error @Line:7   Expected an operand
Error @Line:9   Badly formatted number - 5,
Error @Line:9   Extra operand 
Error @Line:10  Bogous label name
Error @Line:11  Bogus mnemonic
Error @Line:12  Bogus mnemonic


4)
#Input:asm memcpy.asm
#Output: 
a. memcpy.l 
b. memcpy.log (without any error messages)
c. memcpy.o

5)
#Input:asm triangle.asm
#Output: 
a. triangle.l 
b. triangle.log (without any error messages)
c. triangle.o



2. The emulator:

I have tested three examples shown as bellow. The errors only occurred in the file test3.o and 
the program immediately stopped with an error message on the screen. 

The others were tested without any errors detected and produced execution data both on the screen and 
output text file for each example according to the usage instruction. 

I have also added the functionality to pass multiple operations while emulation like -trace -before -disc etc can be passed simultaneously.

After running the bin file given, the trace data are the same as 
the implemented result from my own program.

1)
#Input:emu -trace memcpy.asm
#Output: memcpy.txt (trace) and memcpy_dump.txt

2)
#Input: emu -trace triangle.asm
#Output: triangle.txt(trace) and triangle_dump.txt

3)
#Input: emu -disc -trace -before -after bubble.o
#Output: output_bubble.txt(disc and trace) and bubble_dump.txt

4)
#Input: emu -after Sum_Of_First_Natural_Numbers.o
#Output: sum_dump.txt 
