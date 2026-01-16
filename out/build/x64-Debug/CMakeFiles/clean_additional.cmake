# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MadNotesCpp_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MadNotesCpp_autogen.dir\\ParseCache.txt"
  "MadNotesCpp_autogen"
  )
endif()
