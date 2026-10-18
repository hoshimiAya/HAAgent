# CMakeList.txt: HAAgent 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.23)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (HAAgent)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 平台可选 GUI 程序模式
if(WIN32)
    # set(CMAKE_WIN32_EXECUTABLE ON)
endif()

# 查找线程库
find_package(Threads REQUIRED)
# 查找 Boost 库，确保包含 system 组件
find_package(Boost REQUIRED COMPONENTS system)

# 将源代码添加到此项目的可执行文件。
add_executable (HAAgent
  "src/main.cpp"
  "src/core/server.cpp"
  "src/core/session.cpp"
  "src/http/http_client.cpp"
  "src/llm/chat_engine.cpp"
  "src/llm/chat_session_manager.cpp"
  "src/llm/llm_client.cpp"
  "src/routing/router.cpp"
)

# 添加头文件
target_sources(HAAgent PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      "src/core/server.hpp"
      "src/core/session.hpp"
      "src/http/http_client.hpp"
      "src/llm/chat_context.hpp"
      "src/llm/chat_engine.hpp"
      "src/llm/chat_session_manager.hpp"
      "src/llm/llm_client.hpp"
      "src/routing/router.hpp"
)

# 添加头文件路径
target_include_directories(HAAgent PRIVATE
)

# 链接依赖库
target_link_libraries(HAAgent PRIVATE
  Boost::system
  Threads::Threads
)

# 客户端
add_executable (HAAgentClient
  "src/client/client.cpp"
  "src/client/chat_client.cpp"
  "src/http/http_client.cpp"
)

# 添加头文件
target_sources(HAAgentClient PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
       "src/client/chat_client.hpp"
       "src/http/http_client.hpp"
)

# 链接依赖库
target_link_libraries(HAAgentClient PRIVATE
  Boost::system
  Threads::Threads
)

# 安装规则：仅安装可执行文件
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
