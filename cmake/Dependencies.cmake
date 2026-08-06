include(FetchContent)

set(MWTL_WTL_SOURCE_DIR "" CACHE PATH "Path to an existing official WTL source tree")
set(MWTL_WIL_SOURCE_DIR "" CACHE PATH "Path to an existing Microsoft WIL source tree")

set(MWTL_WTL_REPOSITORY "https://git.code.sf.net/p/wtl/git")
set(MWTL_WTL_TAG "Release_10.0")
set(MWTL_WTL_COMMIT "0e39a4d013fc56a854cb6f64dddee582db5601ab")

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
    if(NOT TARGET WTL::WTL)
        if(MWTL_WTL_SOURCE_DIR)
            mwtl_add_wtl_target("${MWTL_WTL_SOURCE_DIR}")
        else()
            message(STATUS "Fetching official WTL ${MWTL_WTL_TAG} at ${MWTL_WTL_COMMIT}")
            FetchContent_Declare(mwtl_wtl
                GIT_REPOSITORY "${MWTL_WTL_REPOSITORY}"
                GIT_TAG "${MWTL_WTL_COMMIT}"
                GIT_SHALLOW FALSE
                GIT_PROGRESS TRUE)
            FetchContent_GetProperties(mwtl_wtl)
            if(NOT mwtl_wtl_POPULATED)
                FetchContent_Populate(mwtl_wtl)
            endif()
            mwtl_add_wtl_target("${mwtl_wtl_SOURCE_DIR}")
        endif()
    endif()

    if(NOT TARGET WIL::WIL)
        if(MWTL_WIL_SOURCE_DIR)
            mwtl_add_wil_target("${MWTL_WIL_SOURCE_DIR}")
        else()
            message(STATUS "Fetching Microsoft WIL ${MWTL_WIL_TAG} at ${MWTL_WIL_COMMIT}")
            FetchContent_Declare(mwtl_wil
                GIT_REPOSITORY "${MWTL_WIL_REPOSITORY}"
                GIT_TAG "${MWTL_WIL_COMMIT}"
                GIT_SHALLOW FALSE
                GIT_PROGRESS TRUE)
            FetchContent_GetProperties(mwtl_wil)
            if(NOT mwtl_wil_POPULATED)
                FetchContent_Populate(mwtl_wil)
            endif()
            mwtl_add_wil_target("${mwtl_wil_SOURCE_DIR}")
        endif()
    endif()
endfunction()
