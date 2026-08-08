file(READ "${PROJECT_ROOT}/CMakeLists.txt" cmake_text)
file(READ "${PROJECT_ROOT}/CHANGELOG.md" changelog_text)
file(READ "${PROJECT_ROOT}/.github/workflows/release.yml" release_workflow_text)
file(READ "${PROJECT_ROOT}/.github/workflows/ci.yml" ci_workflow_text)

string(REGEX MATCH "project\\(mwtl VERSION ([0-9]+\\.[0-9]+\\.[0-9]+)" _ "${cmake_text}")
set(project_version "${CMAKE_MATCH_1}")
if(project_version STREQUAL "")
    message(FATAL_ERROR "Could not read the mwtl project version")
endif()
if(NOT changelog_text MATCHES "## ${project_version} - (Unreleased|[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9])")
    message(FATAL_ERROR "CHANGELOG does not contain project version ${project_version}")
endif()
if(NOT EXISTS "${PROJECT_ROOT}/cmake/mwtlConfig.cmake.in" OR
   NOT EXISTS "${PROJECT_ROOT}/.github/workflows/release.yml")
    message(FATAL_ERROR "Release package metadata is incomplete")
endif()
if(NOT EXISTS "${PROJECT_ROOT}/tests/consumer_05_compat.cpp")
    message(FATAL_ERROR "0.5 source compatibility fixture is missing")
endif()
if(NOT EXISTS "${PROJECT_ROOT}/tests/verify_coverage.ps1" OR
   NOT ci_workflow_text MATCHES "MinimumPercent 75" OR
   NOT ci_workflow_text MATCHES "output-format cobertura")
    message(FATAL_ERROR "Native source coverage gate is incomplete")
endif()
foreach(package_document IN ITEMS
        README.md CHANGELOG.md SECURITY.md docs/api.md docs/design.md
        docs/stability.md docs/migration-0.6.md
        docs/release-checklist-0.6.0.md docs/accessibility.md
        docs/reference.md)
    if(NOT cmake_text MATCHES "install\\([^)]*${package_document}")
        message(FATAL_ERROR
            "Release package does not install ${package_document}")
    endif()
endforeach()
foreach(architecture IN ITEMS x64 ARM64)
    if(NOT release_workflow_text MATCHES "architecture: ${architecture}")
        message(FATAL_ERROR "Release workflow does not package ${architecture}")
    endif()
endforeach()
if(NOT release_workflow_text MATCHES [[gh release create.*artifacts/\*]])
    message(FATAL_ERROR "Release workflow does not publish aggregated artifacts")
endif()
