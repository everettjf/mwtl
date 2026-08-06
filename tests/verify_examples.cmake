set(example_names
    hello
    application
    window
    native_message
    keyboard
    mouse
    resize
    timer
    paint
    minmax
    close_policy
    window_state)

file(READ "${PROJECT_ROOT}/README.md" root_readme)
file(READ "${PROJECT_ROOT}/examples/README.md" examples_readme)
file(READ "${PROJECT_ROOT}/examples/CMakeLists.txt" examples_cmake)

foreach(example_name IN LISTS example_names)
    if(NOT EXISTS "${PROJECT_ROOT}/examples/${example_name}/main.cpp")
        message(FATAL_ERROR "example source is missing: ${example_name}")
    endif()
    if(NOT EXISTS "${PROJECT_ROOT}/examples/${example_name}/CMakeLists.txt")
        message(FATAL_ERROR "example CMakeLists is missing: ${example_name}")
    endif()
    if(NOT examples_cmake MATCHES "add_subdirectory\\(${example_name}\\)")
        message(FATAL_ERROR "example is not managed by examples/CMakeLists.txt: ${example_name}")
    endif()
    if(NOT root_readme MATCHES "examples/${example_name}")
        message(FATAL_ERROR "root README does not catalog example: ${example_name}")
    endif()
    if(NOT examples_readme MATCHES "`${example_name}`")
        message(FATAL_ERROR "examples README does not catalog example: ${example_name}")
    endif()
endforeach()

foreach(image_name IN ITEMS paint-demo.png native-message-demo.png timer-demo.png)
    if(NOT EXISTS "${PROJECT_ROOT}/docs/images/${image_name}")
        message(FATAL_ERROR "README screenshot is missing: ${image_name}")
    endif()
    if(NOT root_readme MATCHES "docs/images/${image_name}")
        message(FATAL_ERROR "README does not reference screenshot: ${image_name}")
    endif()
endforeach()

list(LENGTH example_names example_count)
if(example_count LESS 10)
    message(FATAL_ERROR "fewer than 10 examples are cataloged")
endif()
