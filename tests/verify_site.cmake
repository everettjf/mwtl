foreach(required IN ITEMS
        index.html
        building.html
        components/index.html
        components/application.html
        components/controls.html
        components/common-controls.html
        components/window.html
        components/hello.html
        assets/mwtl-mark.svg
        assets/site.js
        styles.css
        .nojekyll)
    if(NOT EXISTS "${SITE_ROOT}/${required}")
        message(FATAL_ERROR "required Pages file is missing: ${required}")
    endif()
endforeach()

file(READ "${SITE_ROOT}/styles.css" site_css)
if(NOT site_css MATCHES "@media \\(max-width: 480px\\)" OR
   NOT site_css MATCHES "flex: 1 0 100%" OR
   NOT site_css MATCHES "overflow-wrap: anywhere")
    message(FATAL_ERROR "narrow-screen layout guard is missing")
endif()

file(READ "${SITE_ROOT}/assets/site.js" site_script)
if(NOT site_script MATCHES "aria-pressed" OR
   NOT site_script MATCHES "Switch to.*theme")
    message(FATAL_ERROR "theme toggle state is not accessible")
endif()
if(NOT site_script MATCHES "showModal" OR
   NOT site_script MATCHES "target=\\\"_blank\\\"")
    message(FATAL_ERROR "example preview dialog is missing")
endif()

file(GLOB_RECURSE html_files "${SITE_ROOT}/*.html")
foreach(html_file IN LISTS html_files)
    file(READ "${html_file}" html)
    if(NOT html MATCHES "styles\\.css\\?v=20260808h" OR
       NOT html MATCHES "assets/site\\.js\\?v=20260808h")
        message(FATAL_ERROR "stale or inconsistent site asset version in ${html_file}")
    endif()
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
