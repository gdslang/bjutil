cmake_minimum_required (VERSION 2.8)
project (bjutil)

set(BJUTIL_MAJOR_VERSION 1)
set(BJUTIL_MINOR_VERSION 1)
set(BJUTIL_PATCH_VERSION 0)
set(BJUTIL_VERSION
  ${BJUTIL_MAJOR_VERSION}.${BJUTIL_MINOR_VERSION}.${BJUTIL_PATCH_VERSION})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/bjutil)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

file(GLOB_RECURSE SOURCES
    "src/**.cpp"
)

add_library(bjutil ${SOURCES})
set_property(TARGET bjutil PROPERTY CXX_STANDARD 14)
target_link_libraries(bjutil elf)
target_include_directories(bjutil PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/>  # <prefix>/include/
)

install(TARGETS bjutil
  # IMPORTANT: Add the library to the "export-set"
  EXPORT bjutilTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/"
    COMPONENT dev)

# Add all targets to the build-tree export set
export(TARGETS bjutil
  FILE "${PROJECT_BINARY_DIR}/bjutilTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE bjutil)

# Create the bjutilConfig.cmake and bjutilConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(bjutilConfig.cmake.in
  "${PROJECT_BINARY_DIR}/bjutilConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${FOOBAR_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(bjutilConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/bjutilConfig.cmake" @ONLY)
# ... for both
configure_file(bjutilConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/bjutilConfigVersion.cmake" @ONLY)

# Install the bjutilConfig.cmake and bjutilConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/bjutilConfig.cmake"
  "${PROJECT_BINARY_DIR}/bjutilConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT bjutilTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)
