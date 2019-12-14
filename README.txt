/*******************************************************************************
* File name: README.txt                                                          *
* Project: CS 460 Project 3 Fall 2019                                          *
* Author(s) of Project 2: Haley Psomas-Sheridan, Gleb Krunchinin, Alexandria Labossiere  *
* Editors(s): Elizabeth Axworthy, Alexandria Labossiere, Roberto Rojas      *
* Date: 12/13/19                                                               *
* Description: This file contains README info                                  *
*******************************************************************************/

This project outputs a list of rules used to <input_filename>.p2.
This project outputs the original source code with lexical and syntactical
error messages to a listing file <input_filename>.lst.
This project outputs rest of output to a begugging file <input_filename>.dbg.

The project is run using the make command and ./P3.out <input_filename>.ss
The input file must have the extension '.ss'

This program will convert the .ss file into a .cpp file and will also privide the equivalent expected output. 

An example of running this program would look like:
          make
          ./P3.out test.ss
          
          After running that, test.cpp, test.p1, test.p2, test.dgb, and test.lst will be created. 
          When opening the test.cpp,  you will find that it is very similar if not the same as test.ss.
