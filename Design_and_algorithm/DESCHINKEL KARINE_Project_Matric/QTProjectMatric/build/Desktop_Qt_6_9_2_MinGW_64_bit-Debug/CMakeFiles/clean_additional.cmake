# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SHIRAZ-KHAN_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SHIRAZ-KHAN_autogen.dir\\ParseCache.txt"
  "SHIRAZ-KHAN_autogen"
  )
endif()
