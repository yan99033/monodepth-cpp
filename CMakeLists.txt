SET(PROJECT_NAME monodepth)
PROJECT(${PROJECT_NAME})
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.3)
SET(CMAKE_BUILD_TYPE Release) # Release, RelWithDebInfo
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMakeModules/")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

# Find package
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)

# Find package for TensofFlow
FIND_PACKAGE(Protobuf REQUIRED)
FIND_PACKAGE(TensorFlow REQUIRED)

INCLUDE_DIRECTORIES(
  include
  ${EIGEN3_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${PROTOBUF_INCLUDE_DIRS}
  ${TensorFlow_INCLUDE_DIRS}
)

# Create monodepth library
ADD_LIBRARY(${PROJECT_NAME}_static STATIC src/monodepth.cpp)
ADD_LIBRARY(${PROJECT_NAME} SHARED src/monodepth.cpp)
# Link the libraries to the library
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_static
    ${OpenCV_LIBS}
    ${PROTOBUF_LIBRARIES}
    ${TensorFlow_LIBRARIES}
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${PROTOBUF_LIBRARIES}
    ${TensorFlow_LIBRARIES}
)


# Make the static and shared libraries coexist in the output folder
SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# Make static library available for client
# ('ARCHIVE' keyword required when building static library)
INSTALL(TARGETS ${PROJECT_NAME}_static ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
INSTALL(FILES include/monodepth/monodepth.h DESTINATION include/monodepth)

# Create Executables (Test only)
ADD_EXECUTABLE(inference_monodepth src/inference_monodepth.cpp)
TARGET_LINK_LIBRARIES(inference_monodepth ${PROJECT_NAME})
