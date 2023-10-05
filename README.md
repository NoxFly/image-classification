# Image classification

Tried to implement a small image analyzer which trains then sorts images depending categories.<br>
On this branch, the tests are for the **Garbage Classification AI Challenge** from [Dockship website](https://dockship.io/challenges/5f74153dfbaa7b0393ff19ea/garbage-classification-ai-challenge);

All IDE configuration files are exclude from the git, so you can setup the project as you want, on Linux and Windows.

## How to run

You have to run from the root directory of this project.<br>
Ensure to have linked OpenCV 4.8, and added dlls if you're on Windows.

## Execution results

Setup used :
- i5-10400F 2.90 GHz (12 cores)
- 32 Go 2666 MHz
- Windows 10
- Visual Studio 2022 17.7.4
- OpenCV 4.8.1
- C++ 17

All execution times are in milliseconds.<br>
All accuracies are in %.<br>
All runs are in release mode.<br>
Use of the LBP and the L1 distance algorithms.<br>
14 histograms are generated from an image (divided by 3x3 then 2x2 then 1x1).<br>
No CNN is used.

> No results available for now.