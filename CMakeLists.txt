cmake_minimum_required(VERSION 3.6)
project(ElectronicComp)                                # project name
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# define variables
# set(var value)
set(SOURCE_FILES main.cpp QuestionOne.cpp QuestionOne.h Uart.cpp Uart.h Blob.cpp Blob.h flowctl.cpp serial_cmd.cpp
        question_one_routine.cpp question_three_routine.cpp)
#set(OPENCV "/usr/local/lib") # 引号应该可以使得空格取消分割的作用
# opencv的所有lib
set(OPENCV_LIB opencv_imgcodecs opencv_calib3d opencv_core opencv_features2d opencv_flann opencv_highgui
        opencv_imgproc opencv_ml opencv_objdetect opencv_photo opencv_stitching
        opencv_superres opencv_video opencv_videoio opencv_videostab rt pthread m dl)
# opencv_contrib opencv_gpu opencv_legacy opencv_nonfree opencv_ocl opencv_ts
# include_direcctory 和 link_directorys 必须在定义相关的target(add_executable or add_library)之前
# target不会到在他后面定义的directory中寻找.h文件和.lib文件，因此错序可能会报错
# 指定搜索.h file的目录
include_directories("/usr/local/include/opencv4")     # include directory for all targets
# target_include_directories(target dir1 dir2 ...)                  # include directory for specified target

# 指定搜索.lib file的目录
#link_directories(${OPENCV}/lib)   # libraray search path
link_directories("/opt/cuda/lib64")   # libraray search path
link_directories("/usr/local/lib")   # libraray search path
# link_libarrays(lib1, lib2...)                         # link library to all targets

# 指定从哪些源代码文件中生成exe或lib
# add_library(target source1, source2...)                 # compiler sources to a library named target, not a exe
add_executable(ElectronicComp ${SOURCE_FILES})         # compiler sources to a exe named target, not a library

target_link_libraries(ElectronicComp ${OPENCV_LIB})    # link library to specified target. must after define target opencv_adaboost
