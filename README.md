# CIS*3090 Parallel Programming

These are my assignments for CIS*3090 which I took in my Fall 2024 semester. I scored perfectly on the programming portions of each assignment and got a 100% overall in the class.


>Each assignment is in its respective folder. Anything regarding timings as well as the shell scripts were for gathering timing data for speedup analysis reports and are not necessary for the code to function.

## A1 - Pi
For this assignment we just had to use Pthreads to show a speedup when calculating Pi to the nth decimal place. Instead of properly parallelizing the computation and combining everything together at the end we were instead instructed to just run the parallel algorithm multiple times at varying sizes of n so that the total n was equivalent to the number of decimal places the serial program computed.


## A2 - Encryption
This was a string encryption/decryption project using MPI. A string would be encrypted and then brute force decrypted. We had to show a parallel time speedup for parallel decryption. MPI was used locally but the code reflects multi host functionality where all necessary data is passed to each process.


## A3 - Image Processing
This assignment was about GPGPU programming, specifically image processing using Nvidia Warp. A sharpening and a denoising algorithm were implemented for both RGB and BW images. The actual code is set to use CPU only as that's how it was graded in class but you can see the performance increase when allowing it to actually use the GPU.# CIS3090-Assignments
