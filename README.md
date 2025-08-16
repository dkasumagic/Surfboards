# Surfboards

This project, albeit confusingly named, is a conceptual demonstrate of creating O(1) matrix multiplication for commercial use, in fields like machine learning for example. 

With matrix multiplication being so mundamental to many modern applications of computer science, and its insistance on being a slow process, it is no wonder that solutions to such an issue are very desirable. One such solution, as simplistic as it may seem, is simply performing more parallel tasks with more cores, and so insert the GPU. But yet still, with the demands of potentially massive matrices, this solution has cost and energy limitations. 

It was these thoughts that pushed our team to attempt to produce O(1) matrix multiplication via bespoke circuit boards that "hardcode" the matrix multiplication algorithm. The idea is to have a program write Verilog code, encoding logic gates that describe this algorithm and then convert this Verilog code into a commercially viable circuit diagram that manufacturers could produce

# Features

The program is accessed throught a locally hosted webpage as so:

![alt text]() # insert .png here

The user chooses their desired matrix sizes, with the inner dimensions being locked together to provide valid matrices. The user can also choose the size of their input variables, using the fixed point Q notation, and whether negative values can be used. 

After this, the user presses the generate button and should recieve the applicable circuit diagram.

# Installation/Setup

As the webpage is locally hosted, the user will require Zig to compile the C++ code. 

Once this is done, the user runs the program through the build.zig file (zig build run in powershell) and a web address will be generated. This can be access through the localhost web address on the user's explorer, using the provided port address. 

Use from here is done via the website and is largely believed to be trivial, although it is left as an exercise for the reader to prove this result.

# Examples/Demos

insert demo images here please.

# License

This is an open source project under the MIT license - see the LICENSE file if you're struggling to get to sleep.

# Limitations

This is a flawless and fully complete program ready to enable the AI takeover

# Acknowledgements

Stimulants, sponsors, and sleeplessness.

