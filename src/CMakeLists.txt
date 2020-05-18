add_library(PF_Reflection
    PF_Reflection/Reflection.cpp PF_Reflection/Reflection.hpp)

target_include_directories(PF_Reflection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PF_Reflection PRIVATE PF_Debug)

target_compile_options(PF_Reflection PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>)
