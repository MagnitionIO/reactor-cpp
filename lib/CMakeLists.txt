set(SOURCE_FILES
    action.cc
    assert.cc
    environment.cc
    logical_time.cc
    port.cc
    reaction.cc
    reactor.cc
    scheduler.cc
    time.cc
    multiport.cc
    reactor_element.cc
    )

if(REACTOR_CPP_TRACE)
  set(SOURCE_FILES ${SOURCE_FILES} trace.cc )
endif()

if (DEFINED ReactorSDK)
  set(REACTOR_CPP_INCLUDE "include/${ReactorSDK}")
else()
  set(REACTOR_CPP_INCLUDE "include")
endif()

add_library(${LIB_TARGET} SHARED ${SOURCE_FILES})
target_include_directories(${LIB_TARGET} PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
  "$<BUILD_INTERFACE:${LTTNGUST_INCLUDE_DIRS}>"
  "$<INSTALL_INTERFACE:${REACTOR_CPP_INCLUDE}>"
  PRIVATE src)

if(MSVC)
  target_compile_options(${LIB_TARGET} PRIVATE /W4 /WX)
else()
  target_compile_options(${LIB_TARGET} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

if(${Backtrace_FOUND})
  target_include_directories(${LIB_TARGET} PRIVATE ${Backtrace_INCLUDE_DIRS})
  target_link_libraries(${LIB_TARGET} ${Backtrace_LIBRARY})
endif()

target_link_libraries(${LIB_TARGET} ${CMAKE_THREAD_LIBS_INIT})
if(REACTOR_CPP_TRACE)
  target_link_libraries(${LIB_TARGET} LTTng::UST)
endif()

# Generates Compilation Database for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_target_properties(${LIB_TARGET} PROPERTIES
    VERSION ${PROJECT_VERSION}
    CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY}"
    SOVERSION 1)

# if (APPLE)
#     set_property(TARGET ${LIB_TARGET} PROPERTY PREFIX "lib")
#     set_property(TARGET ${LIB_TARGET} PROPERTY OUTPUT_NAME "${LIB_TARGET}")
#     set_property(TARGET ${LIB_TARGET} PROPERTY SUFFIX ".so")
# endif()

if(REACTOR_CPP_INSTALL)
  if(DEFINED ReactorSDK)
    install(FILES "${PROJECT_BINARY_DIR}/include/reactor-cpp/config.hh" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${ReactorSDK}/reactor-cpp")

    install(TARGETS ${LIB_TARGET} EXPORT ${LIB_TARGET}Config
      ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
      LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
      RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL)

    install(EXPORT ${LIB_TARGET}Config DESTINATION share/${ReactorSDK}/${LIB_TARGET}/cmake)

    export(TARGETS ${PROJECT_NAME} FILE ${ReactorSDK}/${LIB_TARGET}Config.cmake)
  else()
    install(FILES "${PROJECT_BINARY_DIR}/include/reactor-cpp/config.hh" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/reactor-cpp")

    install(TARGETS ${LIB_TARGET} EXPORT ${LIB_TARGET}Config
      ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
      LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
      RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL)

    install(EXPORT ${LIB_TARGET}Config DESTINATION share/${LIB_TARGET}/cmake)

    export(TARGETS ${PROJECT_NAME} FILE ${LIB_TARGET}Config.cmake)
  endif()
endif()
