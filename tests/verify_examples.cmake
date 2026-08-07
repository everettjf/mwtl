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
    window_state
    dpi
    window_options
    wait_aware
    wakeup
    com_sta
    controls
    self_drawn_host
    system_lifecycle)

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
    if(NOT root_readme MATCHES "examples/${example_name}/main.cpp")
        message(FATAL_ERROR "root README does not link to example source: ${example_name}")
    endif()
    if(NOT examples_readme MATCHES "`${example_name}`")
        message(FATAL_ERROR "examples README does not catalog example: ${example_name}")
    endif()
    string(REPLACE "_" "-" image_slug "${example_name}")
    set(image_path "docs/images/examples/${image_slug}.png")
    if(NOT EXISTS "${PROJECT_ROOT}/${image_path}")
        message(FATAL_ERROR "README screenshot is missing: ${image_path}")
    endif()
    if(NOT root_readme MATCHES "${image_path}")
        message(FATAL_ERROR "README does not reference screenshot: ${image_path}")
    endif()
endforeach()

if(NOT EXISTS "${PROJECT_ROOT}/docs/images/mwtl-mark.svg")
    message(FATAL_ERROR "README project mark is missing")
endif()
if(NOT root_readme MATCHES "docs/images/mwtl-mark.svg")
    message(FATAL_ERROR "README does not reference the project mark")
endif()

list(LENGTH example_names example_count)
if(example_count LESS 20)
    message(FATAL_ERROR "fewer than 20 examples are cataloged")
endif()
