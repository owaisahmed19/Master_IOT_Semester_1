cmake_minimum_required(VERSION 3.10)
project(VideoDecoder LANGUAGES CXX CUDA)

# 1. Find the packages
find_package(OpenCV REQUIRED)
find_package(CUDA REQUIRED)

# 2. Create the executable (This MUST be before linking)
add_executable(unscrambler unscrambler.cu)

# 3. Link the libraries
# We use variables ${...} to avoid typos and target errors
target_link_libraries(unscrambler
    ${OpenCV_LIBS}
    ${CUDA_LIBRARIES}
)

