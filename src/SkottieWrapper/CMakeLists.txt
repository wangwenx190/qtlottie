
include_directories(E:\\skia)
link_directories(E:\\skia\\out\\skottie)

set(SOURCE
    SkottieWrapper.cpp)

add_library(SkottieWrapper SHARED ${SOURCE})
if(WIN32)
    set_property(TARGET SkottieWrapper PROPERTY
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(SKOTTIEWRAPPER_LIBRARY_BUILD)
    target_link_libraries(SkottieWrapper PRIVATE
        skia.lib
        skottie.lib
        sksg.lib
        skshaper.lib
        Opengl32.lib)
endif()
