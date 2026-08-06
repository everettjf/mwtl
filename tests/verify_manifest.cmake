if(NOT MT_TOOL OR NOT EXISTS "${MT_TOOL}")
    message(FATAL_ERROR "Windows manifest tool was not found: ${MT_TOOL}")
endif()

if(NOT EXECUTABLE OR NOT EXISTS "${EXECUTABLE}")
    message(FATAL_ERROR "hello executable was not found: ${EXECUTABLE}")
endif()

execute_process(
    COMMAND "${MT_TOOL}" -nologo "-inputresource:${EXECUTABLE};#1" "-out:${OUTPUT}"
    RESULT_VARIABLE mt_result
    ERROR_VARIABLE mt_error)
if(NOT mt_result EQUAL 0)
    message(FATAL_ERROR "mt.exe could not extract the embedded manifest: ${mt_error}")
endif()

file(READ "${OUTPUT}" manifest)
if(NOT manifest MATCHES "PerMonitorV2, PerMonitor")
    message(FATAL_ERROR "embedded manifest does not declare PerMonitorV2")
endif()
if(NOT manifest MATCHES "true/pm")
    message(FATAL_ERROR "embedded manifest does not contain the compatibility DPI declaration")
endif()
