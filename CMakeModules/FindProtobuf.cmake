include(FindPackageHandleStandardArgs)
# include(Protobuf_VERSION)
unset(PROTOBUF_FOUND)

find_path(Protobuf_INCLUDE_DIR
        NAMES
        protobuf
        HINTS
        /usr/local/include/google)

find_library(Protobuf_LIBRARY NAMES protobuf
        HINTS
        /usr/local/lib)

# set Protobuf_FOUND
find_package_handle_standard_args(Protobuf DEFAULT_MSG Protobuf_INCLUDE_DIR Protobuf_LIBRARY)

# set external variables for usage in CMakeLists.txt
if(PROTOBUF_FOUND)
    set(PROTOBUF_LIBRARIES ${Protobuf_LIBRARY})
    set(PROTOBUF_INCLUDE_DIRS ${Protobuf_INCLUDE_DIR})
    message(STATUS "Protobuf found (include: ${PROTOBUF_INCLUDE_DIRS})")
    message(STATUS "Protobuf found (library: ${PROTOBUF_LIBRARIES})")
endif()

# hide locals from GUI
# mark_as_advanced(Protobuf_INCLUDE_DIR)
