#add_library(add_ai_plane SHARED add_ai_plane.c)
add_executable(metar_decode metar_decode.c)
target_link_libraries(metar_decode PRIVATE dxatc-engine dxatc-utils)
