cmake_minimum_required(VERSION 3.25)
project(RobotEscapeApp)
add_executable(robot-escape
        src/main.cpp)
target_compile_features(robot-escape PRIVATE cxx_std_20)
target_link_libraries(robot-escape PRIVATE erbsland-configuration-parser)
