cmake_minimum_required(VERSION 3.14)
project(new400Final)

set(CMAKE_CXX_STANDARD 14)


find_package( OpenCV REQUIRED )
add_executable(new400Final main.cpp OTSU/solutionOneOTSU.h OTSU/solutionOneOTSU.cpp BCV/solutionThreeBCV.h BCV/solutionThreeBCV.cpp Maximise_Busyness/solutionTwoMaximiseBusyness.h Maximise_Busyness/solutionTwoMaximiseBusyness.cpp Dynamic_Programming/DP.h Dynamic_Programming/DP.cpp SOL5/sol5.cpp SOL5/sol5.h)
target_link_libraries( new400Final ${OpenCV_LIBS} )