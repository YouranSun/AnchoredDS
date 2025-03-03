add_library(dssolver STATIC
    dssolver.cpp
    dssolver.h
)

add_library(fdp STATIC
    fdp.cpp
    fdp.h
)

add_library(fista STATIC
    fista.cpp
    fista.h
)

target_link_libraries(fista PRIVATE dssolver)
target_link_libraries(fdp PRIVATE dssolver)