include_guard()
cmake_minimum_required(VERSION 3.14)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(
  MyAppFuzzer
  VERSION 1.0
  DESCRIPTION "Ledger App Fuzzer"
  LANGUAGES C)

if(NOT DEFINED BOLOS_SDK)
  message(FATAL_ERROR "BOLOS_SDK must be defined, CMake will exit.")
  return()
endif()

include(${BOLOS_SDK}/fuzzing/cmake/LedgerAppFuzz.cmake)
ledger_fuzz_setup()

set(APP_SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)

file(GLOB_RECURSE C_SOURCES
  "${APP_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/mock/*.c"
)
list(REMOVE_ITEM C_SOURCES "${APP_SOURCE_DIR}/src/app_main.c")

absolution_add_fuzzer(
    NAME fuzz_globals
    TARGETS
        ${C_SOURCES}
        ${LEDGER_FUZZ_COMMON_SOURCES}
    HARNESS
        ${CMAKE_SOURCE_DIR}/harness/fuzz_dispatcher.c
    ENTRY
        fuzz_entry
    INVARIANT
        ${CMAKE_SOURCE_DIR}/invariants/fuzz_globals.zon
    INCLUDE_DIRECTORIES
        ${APP_SOURCE_DIR}/src/
        ${CMAKE_SOURCE_DIR}/mock/
        ${CMAKE_SOURCE_DIR}
        # TODO: add app-specific include directories here.
    COMPILE_DEFINITIONS
        # TODO: add app-specific defines here (for example HAVE_SWAP).
    LINK_LIBRARIES
        secure_sdk
)
