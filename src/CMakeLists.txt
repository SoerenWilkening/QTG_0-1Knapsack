add_library(common common/knapsack.c common/stategen.c common/syslinks.c)
target_include_directories(common PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(common PUBLIC ${GSL_LIBRARIES} ${gmp_LIBRARIES})

add_library(combo combo/combo.c combo/combowrp.c)
target_include_directories(combo PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(combo PUBLIC common)

add_library(qtg_lib simulation/simulate.c simulation/qtgcount.c)
target_include_directories(qtg_lib PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(qtg_lib PUBLIC common combo)

add_library(generator generator/jooken_generator.cpp)
target_include_directories(generator PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(generator PUBLIC common)

pybind11_add_module(_qtg_bindings bindings/qtg_bindings.cpp)
target_include_directories(_qtg_bindings PUBLIC ${INCLUDE_DIRECTORY})
target_link_libraries(_qtg_bindings PUBLIC common generator)

# just more extensive warnings to spot bugs
target_compile_options(
        _qtg_bindings PRIVATE "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>")
target_compile_definitions(_qtg_bindings PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)
# move the binary `_bindings` to its destination in the Python module
install(TARGETS _qtg_bindings DESTINATION ./pysrc/qtg/bindings/)