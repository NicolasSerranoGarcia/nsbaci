# ------------------------------------------------------------------------------
# CMakeLists.txt for the runtimeView submodule.
#
# Author: Nicolás Serrano García
# Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT License.
# ------------------------------------------------------------------------------

add_library(runtimeView STATIC
  runtimeView.cpp
  runtimeView.h
)

target_link_libraries(runtimeView PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(runtimeView PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/source/types
)
