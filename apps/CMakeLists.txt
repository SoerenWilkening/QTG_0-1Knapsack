add_executable(cmbcount cmbcount.c)
target_link_libraries(cmbcount combo)
target_include_directories(cmbcount PUBLIC ${INCLUDE_DIRECTORY})

add_executable(simulation main.c)
target_link_libraries(simulation qtg_lib)
target_include_directories(simulation PUBLIC ${INCLUDE_DIRECTORY})

add_executable(generator generator.cpp)
