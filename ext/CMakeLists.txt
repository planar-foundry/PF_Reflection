if (NOT TARGET PF_Debug)
    add_subdirectory(PF_Debug)
endif()

if (NOT TARGET PF_Test AND ${PFREFLECTION_BUILD_TESTS})
    add_subdirectory(PF_Test)
endif()
