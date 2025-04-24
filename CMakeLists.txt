cmake_minimum_required(VERSION 3.14)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(mygui VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)

#CMAKE_PREFIX_PATH is cmake used to find libraries
set(CMAKE_PREFIX_PATH "/home/z/zhuy/Develop/libtorch" ${CMAKE_PREFIX_PATH})



#find_package
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# set IMGUI related Macro
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_INCLUDE_DIR ${IMGUI_DIR} ${IMGUI_DIR}/backends ${IMGUI_DIR}/misc/fonts)

#set IMPLOT related Macro
set(IMPLOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/implot)


# debug print message
message(STATUS "--------------debug info---------------------")
message(STATUS "glfw3 include dir: ${GLFW_INCLUDEDIR}")
message(STATUS "glfw3 library: ${GLFW_LIBRARIES}")
message(STATUS "OpenGL include dir: ${OPENGL_INCLUDE_DIR}")
message(STATUS "OpenGL library: ${OPENGL_LIBRARIES}")
message(STATUS "--------------debug info---------------------")



# Target
add_executable(mygui main.cpp)
target_sources(mygui PRIVATE 
               ${IMGUI_DIR}/imgui.cpp
               ${IMGUI_DIR}/imgui_draw.cpp
               ${IMGUI_DIR}/imgui_tables.cpp
               ${IMGUI_DIR}/imgui_widgets.cpp
               ${IMGUI_DIR}/imgui_demo.cpp
               ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
               ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
               ${IMPLOT_DIR}/implot.cpp
               ${IMPLOT_DIR}/implot_items.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui_redirect_log.cpp
               )


# mordern way only affect target file
target_include_directories(mygui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include 
                           ${IMGUI_INCLUDE_DIR} ${IMPLOT_DIR})
target_link_libraries(mygui PRIVATE glfw OpenGL)


# debug print message
# message(STATUS "--------------debug info---------------------")
# message(STATUS "glfw dir: ")
# message(STATUS "OpenGL dir: ${OpenGL}")
# message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
# message(STATUS "CMAKE_LIBRARY_PATH: ${CMAKE_LIBRARY_PATH}")
# message(STATUS "--------------debug info---------------------")


# Move the executable to the source dir after build
add_custom_command(TARGET mygui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
        $<TARGET_FILE:mygui>
        ${CMAKE_SOURCE_DIR}/$<TARGET_FILE_NAME:mygui>
    COMMENT "Moving executable to source directory"
)

