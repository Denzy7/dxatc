set(dxatc-utils-hdr
    include/dxatc-utils/vvtor.h
    include/dxatc-utils/macros.h
    include/dxatc-utils/socket.h
)

set(dxatc-utils-src
    src/vvtor.c
)

add_library(dxatc-utils ${dxatc-utils-hdr} ${dxatc-utils-src})
