set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")

find_program(GLSLC_EXECUTABLE glslc REQUIRED)

file(GLOB_RECURSE SHADER_SOURCES
        "${SHADER_SOURCE_DIR}/*.vert"
        "${SHADER_SOURCE_DIR}/*.frag"
        "${SHADER_SOURCE_DIR}/*.geom"
        "${SHADER_SOURCE_DIR}/*.comp"
)

set(SPV_OUTPUT_FILES "")

foreach (SHADER_SRC ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SRC} NAME)
    set(SHADER_OUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT "${SHADER_OUT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_OUTPUT_DIR}"
        COMMAND "${GLSLC_EXECUTABLE}" "${SHADER_SRC}" -o "${SHADER_OUT}"
        DEPENDS "${SHADER_SRC}"
        COMMENT "Compiling shader: ${SHADER_NAME} -> ${SHADER_NAME}.spv"
        VERBATIM
    )

    list(APPEND SPV_OUTPUT_FILES "${SHADER_OUT}")
endforeach()

add_custom_target(compile_shaders DEPENDS ${SPV_OUTPUT_FILES})