set (ReactorSDK_LIB reactor-sdk)

include(GNUInstallDirs)

find_package(reactor-cpp PATHS )

set(REACTOR_CPP_SDK_INCLUDE "include")

add_library(${ReactorSDK_LIB} SHARED magnition-reactor-cpp.cc)

target_link_libraries(${ReactorSDK_LIB} PUBLIC reactor-cpp)

# if (APPLE)
#     set_property(TARGET ${ReactorSDK_LIB} PROPERTY OUTPUT_NAME "${ReactorSDK_LIB}")
#     set_property(TARGET ${ReactorSDK_LIB} PROPERTY SUFFIX ".so")
# endif()

# install(
#     FILES
#     magnition-reactor-cpp.hh
#     magnition_reactor_base.cmake
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${ReactorSDK}/
# )

# install(
#     TARGETS ${ReactorSDK_LIB}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/
# )

install(TARGETS ${ReactorSDK_LIB} EXPORT ${ReactorSDK_LIB}Config
      ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
      LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
      RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL)

install(EXPORT ${ReactorSDK_LIB}Config DESTINATION share/${ReactorSDK_LIB}/cmake)

export(TARGETS ${ReactorSDK_LIB} FILE ${ReactorSDK_LIB}Config.cmake)

# get_target_property(RUNTIME_CPP_INCLUDE_DIRECTORIES ${RUNTIME_TARGET} INCLUDE_DIRECTORIES)

# message("RUNTIME_CPP_INCLUDE_DIRECTORIES: ${RUNTIME_CPP_INCLUDE_DIRECTORIES}")

# if(NOT TARGET uninstall)
#   configure_file(
#     "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
#     "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
#     IMMEDIATE @ONLY)

#   add_custom_target(uninstall
#     COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
# endif()