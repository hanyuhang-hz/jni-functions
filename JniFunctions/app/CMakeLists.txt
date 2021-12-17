message( STATUS "++++++++++++++++++++++++(A)+++++++++++++++++++++++++++")
cmake_minimum_required(VERSION 3.6.0)
enable_language(C ASM CXX)

# todo cmakeList打印在哪里看？？？
message( STATUS "++++++++++++++++++++++++(A)+++++++++++++++++++++++++++")
project(test_jni)
#输出编译信息
set(CMAKE_VERBOSE_MAKEFILE ON)
#设置编译器和相关FLAGS
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")       #调试信息
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")    #开启所有警告
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -v")       #查看调用关系(invocation
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -v -Wl,--no-warn-shared-textrel")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -v -Wl,--no-fatal-warnings")
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -O0")        #调试包不优化
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG ")    #release包优化


message( STATUS "++++++++++++++++++++++++(B)+++++++++++++++++++++++++++")
#添加源文件的头文件搜索路径
#<递归目录树>
macro(RecurseDirectories result currDir)
file(GLOB_RECURSE children  LIST_DIRECTORIES true RELATIVE ${currDir} ${currDir}/*)
set(dirlist ${currDir})
foreach(child ${children})
    if(IS_DIRECTORY ${currDir}/${child})
        list(APPEND dirlist ${currDir}/${child})
    endif()
endforeach()
set(${result} ${dirlist})
endmacro()
RecurseDirectories(SUB_DIRS ${CMAKE_CURRENT_LIST_DIR}/src/main/cpp)
#添加所有的子目录(头文件目录)
include_directories(${SUB_DIRS})
message(STATUS  "SUB_DIRS:" ${SUB_DIRS})
#查找当前目录下的所有源文件
set(CURR_DIR ${CMAKE_CURRENT_LIST_DIR}/src/main/cpp/jni)
file(GLOB_RECURSE SOURCE_FILES ${CURR_DIR} ${CURR_DIR}/*.cpp ${CURR_DIR}/*.c)
message( STATUS ${SOURCE_FILES})


message( STATUS "++++++++++++++++++++++++++++(E)++++++++++++++++++++++++++++++++++" )
# JNI动态库
add_library( # Sets the name of the library.
             ${PROJECT_NAME}
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             ${SOURCE_FILES} )


# 定义链接时的依赖
target_link_libraries(${PROJECT_NAME} android log
        ${TEST_SHARED_LIBRARIES} ${TEST_STATIC_LIBS})


message( STATUS "++++++++++++++++++++++++++++(F)++++++++++++++++++++++++++++++++++" )
# 依赖的动态库需要拷贝到输出目录(TARGET 的输出目录)
# 注意：
# {add_custom_command} 必须在生成目标的 {add_library target_link_libraries}之后调用,不然编译报错
foreach(lib ${TEST_SHARED_LIBRARIES})
    set(THE_LIBRARY ${SHARED_LIBS_DIR}/lib${lib}.so)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD                                                   # Adds a post-build event to ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different                # which executes "cmake - E copy_if_different..."
        ${THE_LIBRARY}                                               # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>                           # <--this is out-file path
    )
endforeach()