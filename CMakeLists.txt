cmake_minimum_required(VERSION 2.8)
project( build )
find_package( OpenCV REQUIRED )
add_executable( build build.cpp )
target_link_libraries( build ${OpenCV_LIBS} )

