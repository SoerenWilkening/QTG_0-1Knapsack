add_library(common common/knapsack.c common/stategen.c common/syslinks.c)
target_include_directories(common PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(common PUBLIC ${GSL_LIBRARIES} ${gmp_LIBRARIES})

add_library(combo combo/combo.c combo/combowrp.c)
target_include_directories(combo PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(combo PUBLIC common)

add_library(qtg_lib simulation/simulate.c simulation/qtgcount.c)
target_include_directories(qtg_lib PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(qtg_lib PUBLIC common combo)