add_executable(mwtl_lifecycle_test lifecycle_test.cpp)
target_compile_features(mwtl_lifecycle_test PRIVATE cxx_std_17)
target_compile_options(mwtl_lifecycle_test PRIVATE /W4 /permissive- /EHsc /utf-8)
target_compile_definitions(mwtl_lifecycle_test PRIVATE MWTL_TESTING)
target_include_directories(mwtl_lifecycle_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(mwtl_lifecycle_test PRIVATE mwtl::mwtl)

foreach(mode IN ITEMS
        normal
        module_init_failure
        loop_registration_failure
        create_failure
        message_failure)
    add_test(NAME mwtl.lifecycle.${mode} COMMAND mwtl_lifecycle_test ${mode})
    set(test_environment "MWTL_TEST_NO_DIALOGS=1")
    if(mode STREQUAL "module_init_failure")
        list(APPEND test_environment "MWTL_TEST_FAIL_MODULE_INIT=1")
    elseif(mode STREQUAL "loop_registration_failure")
        list(APPEND test_environment "MWTL_TEST_FAIL_LOOP_REGISTRATION=1")
    endif()
    set_tests_properties(mwtl.lifecycle.${mode} PROPERTIES
        ENVIRONMENT "${test_environment}"
        TIMEOUT 20)
endforeach()

foreach(header IN ITEMS application window umbrella)
    add_library(mwtl_header_${header} OBJECT header_${header}.cpp)
    target_compile_features(mwtl_header_${header} PRIVATE cxx_std_17)
    target_compile_options(mwtl_header_${header} PRIVATE /W4 /permissive- /EHsc /utf-8)
    target_link_libraries(mwtl_header_${header} PRIVATE mwtl::mwtl)
endforeach()

if(TARGET mwtl_hello)
    set(mwtl_mt_tool "${CMAKE_MT}")
    if(NOT EXISTS "${mwtl_mt_tool}" AND CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
        set(mwtl_mt_tool
            "C:/Program Files (x86)/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/x64/mt.exe")
    endif()
    if(NOT EXISTS "${mwtl_mt_tool}")
        find_program(mwtl_mt_tool NAMES mt.exe)
    endif()
    foreach(example_target IN ITEMS
            mwtl_hello
            mwtl_application_demo
            mwtl_window_demo)
        add_test(
            NAME mwtl.manifest.${example_target}
            COMMAND "${CMAKE_COMMAND}"
                "-DMT_TOOL=${mwtl_mt_tool}"
                "-DEXECUTABLE=$<TARGET_FILE:${example_target}>"
                "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/${example_target}-manifest.xml"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_manifest.cmake")
        set_tests_properties(mwtl.manifest.${example_target} PROPERTIES TIMEOUT 20)
    endforeach()
endif()

add_test(
    NAME mwtl.site_links
    COMMAND "${CMAKE_COMMAND}"
        "-DSITE_ROOT=${PROJECT_SOURCE_DIR}/site"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_site.cmake")
set_tests_properties(mwtl.site_links PROPERTIES TIMEOUT 20)
