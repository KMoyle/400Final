cmake_minimum_required(VERSION 3.13)
project(EGH400_1_2)

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )
add_executable(EGH400_1_2 main.cpp OTSU/solutionOneOTSU.h OTSU/solutionOneOtsu.cpp Maximise_Busyness/solutionTwoMaximiseBusyness.cpp Maximise_Busyness/solutionTwoMaximiseBusyness.h BCV/solutionThreeBCV.h BCV/solutionThreeBCV.cpp)
target_link_libraries( EGH400_1_2 ${OpenCV_LIBS} )

