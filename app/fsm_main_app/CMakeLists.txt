cmake_minimum_required(VERSION 3.13)



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories()
add_library(fsm_main_app
    inc/fsm_main_app.h 
    src/fsm_main_app.c)

target_include_directories(fsm_main_app PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/inc)  


target_link_libraries(fsm_main_app pico_stdlib ) 

