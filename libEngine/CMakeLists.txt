add_library(graphEngineLib STATIC)

target_sources(graphEngineLib
        PRIVATE graph.cpp

        PUBLIC
        FILE_SET HEADERS
        FILES
        graph.hpp)
