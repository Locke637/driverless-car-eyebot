# driverless-car
**Note:This project is based on eyebot system develop by The Univ. of Western Australia, so if you want to use it you should replace the eyebot API code(controlling the motor) into your function.**

Carolo-Cup is an international autonomous driving competition held in German annually. Student teams need to compete with each other about the new ideas and actual performance of their driverless model vehicles. The test platform built by Technische Universität Braunschweig contains different scenarios, so it can well simulate the real-life situation. In other words, although this is a model, the experience gained from solving problems in the model world can be very helpful to the realistic environment and this is the most fascinating part of this game.This project has done the lane detection, traffic sign detection and classification and communication between eyebots and base station.

## How to build
1. stand in your root folder
2. run `mkdir build`
3. run `sh make`
4. if you want to add new source file add it in `CMakeList`. And the source file should be included in `src` folder.
5. the executing file will be stored in the `build` folder. 


lane detection;

traffic sign detection and classification;

using cmake to compile;

