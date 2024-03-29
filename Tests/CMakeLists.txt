find_package(Catch2 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
foreach (file ${SOURCES})
    get_filename_component(filename ${file} NAME_WLE)
    display_header("Creating Makefile of ${filename}")
    add_executable(${filename} ${file})
    add_test(NAME ${filename} COMMAND ${filename})
    target_link_libraries(${filename} PRIVATE ${META_PROJECT_NAME} ${META_PROJECT_NAME}_test)
    target_link_system_libraries(${filename} PRIVATE Catch2 Catch2WithMain)
endforeach ()

if (MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/Tests/netCDFReaderTestBathymetry.nc ${CMAKE_CURRENT_BINARY_DIR}/Tests COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/Tests/netCDFReaderTestDisplacement.nc ${CMAKE_CURRENT_BINARY_DIR}/Tests COPYONLY)
else ()
    configure_file(${CMAKE_SOURCE_DIR}/Tests/netCDFReaderTestBathymetry.nc ${CMAKE_BINARY_DIR}/Tests COPYONLY)

    configure_file(${CMAKE_SOURCE_DIR}/Tests/netCDFReaderTestDisplacement.nc ${CMAKE_BINARY_DIR}/Tests COPYONLY)
endif ()
