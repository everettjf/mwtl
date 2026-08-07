foreach(required IN ITEMS
        index.html
        building.html
        components/index.html
        components/application.html
        components/controls.html
        components/common-controls.html
        components/window.html
        components/hello.html
        styles.css
        .nojekyll)
    if(NOT EXISTS "${SITE_ROOT}/${required}")
        message(FATAL_ERROR "required Pages file is missing: ${required}")
    endif()
endforeach()

file(GLOB_RECURSE html_files "${SITE_ROOT}/*.html")
foreach(html_file IN LISTS html_files)
    file(READ "${html_file}" html)
    string(REGEX MATCHALL "href=\"[^\"]+\"" hrefs "${html}")
    get_filename_component(html_directory "${html_file}" DIRECTORY)

    foreach(href IN LISTS hrefs)
        string(REGEX REPLACE "^href=\"|\"$" "" link "${href}")
        if(link MATCHES "^(https://|http://|mailto:|#)")
            continue()
        endif()
        string(REGEX REPLACE "#.*$" "" link_path "${link}")
        string(REGEX REPLACE "\\?.*$" "" link_path "${link_path}")
        if(link_path STREQUAL "")
            continue()
        endif()
        get_filename_component(target "${html_directory}/${link_path}" ABSOLUTE)
        if(NOT EXISTS "${target}")
            message(FATAL_ERROR "broken local link in ${html_file}: ${link}")
        endif()
    endforeach()
endforeach()
