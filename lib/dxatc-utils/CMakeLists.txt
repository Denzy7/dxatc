set(dxatc-utils-hdr
    include/dxatc-utils/vvtor.h
    include/dxatc-utils/macros.h
    include/dxatc-utils/socket.h
    include/dxatc-utils/str.h
)

set(dxatc-utils-src
    src/vvtor.c
    src/str.c
)

add_library(dxatc-utils ${dxatc-utils-hdr} ${dxatc-utils-src})
