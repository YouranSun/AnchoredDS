cmake_minimum_required(VERSION 3.10)
project(AnchoredDS)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "-fsanitize=undefined")
# set(CMAKE_CXX_FLAGS "-fsanitize=address")
set(EXECUTABLE_OUTPUT_PATH "bin/")


add_subdirectory(src/solvers)
add_subdirectory(src/graph)
add_subdirectory(src/nrcore)
add_subdirectory(src/run)

add_executable(${PROJECT_NAME} src/run/run.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE dssolver)
target_link_libraries(${PROJECT_NAME} PRIVATE fista)
target_link_libraries(${PROJECT_NAME} PRIVATE fdp)
target_link_libraries(${PROJECT_NAME} PRIVATE graph)
target_link_libraries(${PROJECT_NAME} PRIVATE nrcore)
target_link_libraries(${PROJECT_NAME} PRIVATE setting)

add_executable(generator src/run/generator.cpp)
target_link_libraries(generator PRIVATE 
    graph
)