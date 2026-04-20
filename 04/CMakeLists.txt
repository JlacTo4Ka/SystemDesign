cmake_minimum_required(VERSION 3.12...3.31)
project(jwt_auth CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(
    userver
    COMPONENTS core chaotic postgresql mongo
    QUIET
)
if(NOT userver_FOUND)
    download_userver(TRY_DIR third_party/userver)
endif()

find_package(jwt-cpp QUIET)
if (NOT jwt-cpp_FOUND)
  message("Loading JWT-CPP")
  include(cmake/get_cpm.cmake)
  CPMAddPackage(
    NAME jwt-cpp
    GITHUB_REPOSITORY Thalhammer/jwt-cpp
    VERSION 0.7.2
    OPTIONS "JWT_BUILD_EXAMPLES OFF" "JWT_BUILD_TESTS OFF" "JWT_CMAKE_FILES_INSTALL ON"
  )
endif()

userver_setup_environment()

include_directories(src)

add_library(
    ${PROJECT_NAME}_objs OBJECT
    src/auth/jwt_auth_checker.cpp
    src/auth/jwt_auth_factory.cpp
    src/views/auth/login_handler.cpp

    src/views/users/create_handler.cpp
    src/views/users/get_by_login_handler.cpp
    src/views/users/search_handler.cpp
    src/views/files/create_handler.cpp
    src/views/files/get_by_name_handler.cpp
    src/views/files/delete_handler.cpp
    src/views/folders/delete_handler.cpp
    src/views/folders/create_handler.cpp
    src/views/folders/get_all_handler.cpp

    src/views/mongo_folders/create_handler.cpp
    src/views/mongo_folders/get_all_handler.cpp
    src/views/mongo_folders/delete_handler.cpp

    src/storage/postgres_storage.cpp
    src/storage/storage_component.cpp
    src/storage/mongo_folder_storage.cpp
    src/storage/mongo_storage_component.cpp

    src/dto/users/user_dto.cpp
    src/dto/auth/auth_dto.cpp
    src/dto/folders/folder_dto.cpp
    src/dto/files/file_dto.cpp

    src/utils/string_utils.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_objs
    PUBLIC userver::core
    PUBLIC userver::postgresql
    PUBLIC userver::mongo
    PUBLIC jwt-cpp::jwt-cpp
)

target_include_directories(${PROJECT_NAME}_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

file(GLOB_RECURSE SCHEMAS ${CMAKE_CURRENT_SOURCE_DIR}/docs/*.yaml)
userver_target_generate_chaotic(
    ${PROJECT_NAME}-chgen
    GENERATE_SERIALIZERS
    LAYOUT "/components/schemas/([^/]*)/=jwt_auth::{stem}::{0}"
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/src
    SCHEMAS ${SCHEMAS}
    RELATIVE_TO ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC ${PROJECT_NAME}-chgen)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

userver_testsuite_add_simple(REQUIREMENTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/requirements.txt)

include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
    message(STATUS "Set install prefix: $ENV{PREFIX}")
    file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
    set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
)
install(
    FILES ${CONFIGS_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)