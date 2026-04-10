cmake_minimum_required(VERSION 4.3.1)
project(vulkan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL3_ROOT $ENV{HOME}/vendor/sdl3)
set(Vulkan_ROOT $ENV{HOME}/vendor/1.4.341.1/x86_64)
find_package(SDL3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cpp)

add_library(VulkanModule SHARED)
target_sources(VulkanModule
    PUBLIC
    FILE_SET CXX_MODULES
    FILES
    vulkan.cppm
)
target_compile_definitions(VulkanModule
    PUBLIC
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)

target_link_libraries(VulkanModule PUBLIC SDL3::SDL3 Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE VulkanModule)

install(TARGETS VulkanModule ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

