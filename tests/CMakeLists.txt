add_executable(PF_Reflection_UnitTests
    main.cpp
    Prototype_Manual.cpp
    Prototype_Macro.cpp)
target_link_libraries(PF_Reflection_UnitTests PF_Debug PF_Reflection PF_Test)

target_compile_options(PF_Reflection_UnitTests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Wno-invalid-offsetof>)

add_custom_command(TARGET PF_Reflection_UnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:PF_Debug>
    $<TARGET_FILE:PF_Reflection>
    $<TARGET_FILE_DIR:PF_Reflection_UnitTests>)
