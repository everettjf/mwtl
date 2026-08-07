add_executable(mwtl_lifecycle_test lifecycle_test.cpp)
target_compile_features(mwtl_lifecycle_test PRIVATE cxx_std_20)
target_compile_options(mwtl_lifecycle_test PRIVATE /W4 /permissive- /EHsc /utf-8)
target_compile_definitions(mwtl_lifecycle_test PRIVATE MWTL_TESTING)
target_include_directories(mwtl_lifecycle_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(mwtl_lifecycle_test PRIVATE mwtl::mwtl)

foreach(mode IN ITEMS
        normal
        module_init_failure
        loop_registration_failure
        create_failure
        message_failure
        destruction_failure)
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

foreach(header IN ITEMS application dpi message_pump window window_options wakeup umbrella)
    add_library(mwtl_header_${header} OBJECT header_${header}.cpp)
    target_compile_features(mwtl_header_${header} PRIVATE cxx_std_20)
    target_compile_options(mwtl_header_${header} PRIVATE /W4 /permissive- /EHsc /utf-8)
    target_link_libraries(mwtl_header_${header} PRIVATE mwtl::mwtl)
endforeach()

add_library(mwtl_consumer_cpp20 OBJECT consumer_cpp20.cpp)
target_compile_features(mwtl_consumer_cpp20 PRIVATE cxx_std_20)
target_compile_options(mwtl_consumer_cpp20 PRIVATE /W4 /permissive- /EHsc /utf-8)
target_link_libraries(mwtl_consumer_cpp20 PRIVATE mwtl::mwtl)

add_executable(mwtl_dpi_test dpi_test.cpp)
target_compile_features(mwtl_dpi_test PRIVATE cxx_std_20)
target_compile_options(mwtl_dpi_test PRIVATE /W4 /permissive- /EHsc /utf-8)
target_link_libraries(mwtl_dpi_test PRIVATE mwtl::mwtl)
add_test(NAME mwtl.dpi COMMAND mwtl_dpi_test)
set_tests_properties(mwtl.dpi PROPERTIES TIMEOUT 20)

add_executable(mwtl_advanced_lifecycle_test advanced_lifecycle_test.cpp)
target_compile_features(mwtl_advanced_lifecycle_test PRIVATE cxx_std_20)
target_compile_options(mwtl_advanced_lifecycle_test PRIVATE /W4 /permissive- /EHsc /utf-8)
target_compile_definitions(mwtl_advanced_lifecycle_test PRIVATE MWTL_TESTING)
target_include_directories(mwtl_advanced_lifecycle_test PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(mwtl_advanced_lifecycle_test PRIVATE mwtl::mwtl)
foreach(mode IN ITEMS
        custom_window wait_handle wakeup wake_stress wake_latency idle_efficiency com_sta com_conflict
        pump_exception after_dispatch_exception wait_callback_exception
        wait_failure exact_exit dpi_change)
    add_test(NAME mwtl.advanced.${mode} COMMAND mwtl_advanced_lifecycle_test ${mode})
    set_tests_properties(mwtl.advanced.${mode} PROPERTIES
        ENVIRONMENT "MWTL_TEST_NO_DIALOGS=1"
        TIMEOUT 20)
endforeach()

add_executable(mwtl_liney_host_compat_test liney_host_compat_test.cpp)
target_compile_features(mwtl_liney_host_compat_test PRIVATE cxx_std_20)
target_compile_options(mwtl_liney_host_compat_test PRIVATE /W4 /permissive- /EHsc /utf-8)
target_link_libraries(mwtl_liney_host_compat_test PRIVATE mwtl::mwtl)
add_test(NAME mwtl.liney_host_compat COMMAND mwtl_liney_host_compat_test)
set_tests_properties(mwtl.liney_host_compat PROPERTIES
    ENVIRONMENT "MWTL_TEST_NO_DIALOGS=1"
    TIMEOUT 20)

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
            mwtl_window_demo
            mwtl_native_message_demo
            mwtl_keyboard_demo
            mwtl_mouse_demo
            mwtl_resize_demo
            mwtl_timer_demo
            mwtl_paint_demo
            mwtl_minmax_demo
            mwtl_close_policy_demo
            mwtl_window_state_demo
            mwtl_dpi_demo
            mwtl_window_options_demo
            mwtl_wait_aware_demo
            mwtl_wakeup_demo
            mwtl_com_sta_demo
            mwtl_self_drawn_host_demo
            mwtl_system_lifecycle_demo)
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

add_test(
    NAME mwtl.examples.catalog
    COMMAND "${CMAKE_COMMAND}"
        "-DPROJECT_ROOT=${PROJECT_SOURCE_DIR}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/verify_examples.cmake")
set_tests_properties(mwtl.examples.catalog PROPERTIES TIMEOUT 20)
