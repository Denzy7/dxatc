#add_library(add_ai_plane SHARED add_ai_plane.c)
add_executable(metar_decode metar_decode.c)
target_link_libraries(metar_decode PRIVATE dxatc-engine dxatc-utils)

add_executable(metar_decode_xplane metar_decode_xplane.c)
target_link_libraries(metar_decode_xplane PRIVATE dxatc-engine dxatc-utils)

add_executable(airport_xplane_load airport_xplane_load.c)
target_link_libraries(airport_xplane_load PRIVATE dxatc-engine dxatc-utils)
