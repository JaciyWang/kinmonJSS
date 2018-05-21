cmake_minimum_required(VERSION 3.9.1)
project(kinmon C)

set(CMAKE_C_STANDARD 11)

add_executable(basic
        src/readers/standard.c
        src/readers/dataset.c
        src/utils/dump_jss_input.c
        src/cli/cli.c
        src/algos/bigant/bigant.c
        src/config.c
        src/entries/console.c
        src/GeneticAlgo/geneAlgo.c
        )
target_compile_definitions(basic PUBLIC LOG_LEVEL_INFO=1)

#find_package(Doxygen
#        PATHS ./3rdparties/doxygen
#        )
#option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
#
