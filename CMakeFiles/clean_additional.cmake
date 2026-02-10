# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\KalaNetQt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\KalaNetQt_autogen.dir\\ParseCache.txt"
  "KalaNetQt_autogen"
  )
endif()
