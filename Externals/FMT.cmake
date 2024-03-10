CPMAddPackage(
    NAME FMT
    GITHUB_REPOSITORY MethanePowered/FMT
    GIT_TAG 10.0.0
    VERSION 10.0.0
)

set_target_properties(fmt
    PROPERTIES
    FOLDER Externals
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(fmt INTERFACE
        # FIXME: Silence MSVC warnings about use of stdext::checked_array_iterator until migration to C++20:
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    )
endif()
