
# 1. glslc 실행 파일 찾기
find_program(GLSLC_EXECUTABLE glslc HINTS "${VULKAN_SDK_PATH}/bin")
if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "Could not find glslc executable!")
endif()

# 2. 쉐이더 소스 및 출력 경로 설정
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/shaders")
# 바이너리 출력 디렉토리가 없으면 생성
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

# 3. 컴파일할 쉐이더 파일 목록 찾기 (.vert, .frag)
file(GLOB SHADER_SOURCES
        "${SHADER_SOURCE_DIR}/*.vert"
        "${SHADER_SOURCE_DIR}/*.frag"
)
set(ALL_SPV_FILES "")

# 4. 각 쉐이더 파일에 대해 Custom Command 생성
foreach(SOURCE_FILE ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)

    if (${FILE_NAME} MATCHES "\\.vert$")
        string(REPLACE ".vert" "" BASE_NAME ${FILE_NAME})
        set(OUTPUT_NAME "${BASE_NAME}.vert.spv")
    elseif (${FILE_NAME} MATCHES "\\.frag$")
        string(REPLACE ".frag" "" BASE_NAME ${FILE_NAME})
        set(OUTPUT_NAME "${BASE_NAME}.frag.spv")
    else()
        set(OUTPUT_NAME "${FILE_NAME}.spv")
    endif()
    set(SPV_FILE "${SHADER_BINARY_DIR}/${OUTPUT_NAME}")
    add_custom_command(
            OUTPUT ${SPV_FILE}
            COMMAND ${GLSLC_EXECUTABLE} ${SOURCE_FILE} -o ${SPV_FILE}
            DEPENDS ${SOURCE_FILE}
            COMMENT "Compiling shader: ${FILE_NAME} -> ${OUTPUT_NAME}"
    )
    list(APPEND ALL_SPV_FILES ${SPV_FILE})
endforeach()

# 5. 쉐이더 타겟 생성 및 메인 타겟에 의존성 추가
add_custom_target(Shaders ALL DEPENDS ${ALL_SPV_FILES})
