include(FetchContent)

set(MWTL_DEPENDENCY_MODE "AUTO" CACHE STRING
    "Dependency resolution: AUTO, FETCH, or SYSTEM")
set_property(CACHE MWTL_DEPENDENCY_MODE PROPERTY STRINGS AUTO FETCH SYSTEM)

set(MWTL_WTL_SOURCE_DIR "" CACHE PATH "Path to an existing official WTL source tree")
set(MWTL_WIL_SOURCE_DIR "" CACHE PATH "Path to an existing Microsoft WIL source tree")

set(MWTL_WTL_REPOSITORY "https://git.code.sf.net/p/wtl/git")
set(MWTL_WTL_TAG "10.01")
set(MWTL_WTL_COMMIT "011be908a1122e7bc9fd1106ecc48f22f5f86f00")

set(MWTL_WIL_REPOSITORY "https://github.com/microsoft/wil.git")
set(MWTL_WIL_TAG "v1.0.260126.7")
set(MWTL_WIL_COMMIT "cbf677fb0a942557d08fd129f4c106a76247b2ec")

function(mwtl_add_wtl_target source_dir)
    if(NOT EXISTS "${source_dir}/Include/atlapp.h")
        message(FATAL_ERROR "MWTL_WTL_SOURCE_DIR does not contain Include/atlapp.h: ${source_dir}")
    endif()
    add_library(mwtl_wtl_headers INTERFACE)
    add_library(WTL::WTL ALIAS mwtl_wtl_headers)
    target_include_directories(mwtl_wtl_headers SYSTEM INTERFACE "${source_dir}/Include")
endfunction()

function(mwtl_add_wil_target source_dir)
    if(NOT EXISTS "${source_dir}/include/wil/resource.h")
        message(FATAL_ERROR "MWTL_WIL_SOURCE_DIR does not contain include/wil/resource.h: ${source_dir}")
    endif()
    add_library(mwtl_wil_headers INTERFACE)
    add_library(WIL::WIL ALIAS mwtl_wil_headers)
    target_include_directories(mwtl_wil_headers SYSTEM INTERFACE "${source_dir}/include")
endfunction()

function(mwtl_resolve_dependencies)
    string(TOUPPER "${MWTL_DEPENDENCY_MODE}" dependency_mode)
    if(NOT dependency_mode MATCHES "^(AUTO|FETCH|SYSTEM)$")
        message(FATAL_ERROR
            "MWTL_DEPENDENCY_MODE must be AUTO, FETCH, or SYSTEM")
    endif()

    if(NOT TARGET WTL::WTL)
        if(MWTL_WTL_SOURCE_DIR)
            mwtl_add_wtl_target("${MWTL_WTL_SOURCE_DIR}")
        elseif(dependency_mode STREQUAL "SYSTEM")
            find_path(mwtl_wtl_system_dir Include/atlapp.h REQUIRED)
            mwtl_add_wtl_target("${mwtl_wtl_system_dir}")
        else()
            message(STATUS "Fetching official WTL ${MWTL_WTL_TAG} at ${MWTL_WTL_COMMIT}")
            FetchContent_Declare(mwtl_wtl
                GIT_REPOSITORY "${MWTL_WTL_REPOSITORY}"
                GIT_TAG "${MWTL_WTL_COMMIT}"
                GIT_SHALLOW FALSE
                GIT_PROGRESS TRUE
                SOURCE_SUBDIR _mwtl_headers_only)
            FetchContent_MakeAvailable(mwtl_wtl)
            mwtl_add_wtl_target("${mwtl_wtl_SOURCE_DIR}")
        endif()
    endif()

    if(NOT TARGET WIL::WIL)
        if(MWTL_WIL_SOURCE_DIR)
            mwtl_add_wil_target("${MWTL_WIL_SOURCE_DIR}")
        elseif(dependency_mode STREQUAL "SYSTEM")
            find_path(mwtl_wil_include_dir wil/resource.h REQUIRED)
            get_filename_component(mwtl_wil_system_dir
                "${mwtl_wil_include_dir}" DIRECTORY)
            mwtl_add_wil_target("${mwtl_wil_system_dir}")
        else()
            message(STATUS "Fetching Microsoft WIL ${MWTL_WIL_TAG} at ${MWTL_WIL_COMMIT}")
            FetchContent_Declare(mwtl_wil
                GIT_REPOSITORY "${MWTL_WIL_REPOSITORY}"
                GIT_TAG "${MWTL_WIL_COMMIT}"
                GIT_SHALLOW FALSE
                GIT_PROGRESS TRUE
                SOURCE_SUBDIR _mwtl_headers_only)
            FetchContent_MakeAvailable(mwtl_wil)
            mwtl_add_wil_target("${mwtl_wil_SOURCE_DIR}")
        endif()
    endif()

    get_target_property(resolved_wtl_include WTL::WTL INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(resolved_wil_include WIL::WIL INTERFACE_INCLUDE_DIRECTORIES)
    list(GET resolved_wtl_include 0 resolved_wtl_include)
    list(GET resolved_wil_include 0 resolved_wil_include)
    get_filename_component(resolved_wtl_source "${resolved_wtl_include}" DIRECTORY)
    get_filename_component(resolved_wil_source "${resolved_wil_include}" DIRECTORY)
    set(MWTL_RESOLVED_WTL_SOURCE_DIR "${resolved_wtl_source}" PARENT_SCOPE)
    set(MWTL_RESOLVED_WIL_SOURCE_DIR "${resolved_wil_source}" PARENT_SCOPE)
endfunction()
