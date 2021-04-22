# Locates the tensorFlow library and include directories.

include(FindPackageHandleStandardArgs)

find_path(TensorFlow_base
        NAMES
        tensorflow
        third_party
        external
        HINTS
        /usr/local/include/google/tensorflow
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/tensorflow/local/include/google/tensorflow
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tensorflow/local/include/google/tensorflow)

list(APPEND TensorFlow_INCLUDE_DIR ${TensorFlow_base})
list(APPEND TensorFlow_INCLUDE_DIR ${TensorFlow_base}/external/nsync/public)

find_library(TensorFlow_LIBRARIES NAMES tensorflow_cc
        HINTS
        /usr/local/lib)

# set TensorFlow_FOUND
find_package_handle_standard_args(TensorFlow DEFAULT_MSG TensorFlow_INCLUDE_DIR TensorFlow_LIBRARIES)

# set external variables for usage in CMakeLists.txt
if(TENSORFLOW_FOUND)
  set(TensorFlow_LIBRARIES ${TensorFlow_LIBRARIES})
  set(TensorFlow_INCLUDE_DIRS ${TensorFlow_INCLUDE_DIR})
  message(STATUS "TensorFlow found (include: ${TensorFlow_INCLUDE_DIRS})")
  message(STATUS "TensorFlow found (lib: ${TensorFlow_LIBRARIES})")

endif()
