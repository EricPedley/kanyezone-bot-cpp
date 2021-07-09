# C++ OpenCV Game Bot
![project screenshot](https://media.giphy.com/media/w3GSrHSP9LepvCtW0G/giphy.gif)
<br>[Demo Link](http://www.youtube.com/watch?v=yzOuAT0VVvA)
## Description
  This is a C++ application that uses the windows API to take screenshots and simulate keypresses, uses OpenCV to process the screenshots to find the position of all the game elements on the screen, and then does some math to figure out which direction the blue dot should rotate around the center. The game it plays is http://www.kanyezone.com/ and it easily beats any human performance.

  I also wrote a previous version of this project in python: https://github.com/EricPedley/kanyezone-bot
  
  The easiest way to set up this project would be this tutorial for setting up opencv on Windows: https://cuda-chen.github.io/programming/image%20processing/2020/01/21/vscode-with-opencv-cpp-on-windows10-explained.html.
  My setup was identical to that tutorial, except that I removed "-llibopencv_callib3d411" from tasks.json because g++ said it couldn't find that library (even though it was in opencv's bin)
  In the demo video I'm using opencv 4.5.1 built from source.
  
  I'm including my .vscode in this repo because setting up the compiler and dependencies was a headache. If you're cloning this you need to replace the paths to mingw and opencv with the paths on your computer. You can find the downloads for those and setup instructions in the tutorial I linked above.
