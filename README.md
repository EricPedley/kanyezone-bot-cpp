# Bot for playing Kanye Zone

  I'm rewriting [this Python project](https://github.com/EricPedley/kanyezone-bot) in C++ to try making it lag less and to learn C++

  Tutorial for setting up opencv on windows: https://cuda-chen.github.io/programming/image%20processing/2020/01/21/vscode-with-opencv-cpp-on-windows10-explained.html.
  My setup is identical to that tutorial, except that I removed "-llibopencv_callib3d411" from tasks.json because g++ said it couldn't find that library (even though it was in opencv's bin)