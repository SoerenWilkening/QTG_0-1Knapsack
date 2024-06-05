add_library(common common/knapsack.c common/stategen.c common/syslinks.c)
target_include_directories(common PUBLIC ${INCLUDE_DIRECTORY} ${GSL_INCLUDE_DIRS} ${gmp_INCLUDE_DIRS})
target_link_libraries(common PUBLIC ${GSL_LIBRARIES} ${gmp_LIBRARIES})

add_library(map map/map.c)
target_include_directories(map PUBLIC ${INCLUDE_DIRECTORY})

add_library(combo combo/combo.c combo/combowrp.c)
target_include_directories(combo PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(combo PUBLIC common map)

add_library(expknap expknap/expknap.c)
target_include_directories(expknap PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(expknap PUBLIC common map)

add_library(qtg_lib simulation/simulate.c
        simulation/qtgcount.c
        simulation/quantum_branch_and_bound.c)
target_include_directories(qtg_lib PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(qtg_lib PUBLIC common combo)

add_library(generator generator/jooken_generator.cpp)
target_include_directories(generator PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(generator PUBLIC common)

