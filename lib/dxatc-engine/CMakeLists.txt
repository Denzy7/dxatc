set(dxatc-engine-hdr
    include/dxatc-engine
    include/dxatc-engine/metar.h
    include/dxatc-engine/weather.h
    include/dxatc-engine/controller.h
    include/dxatc-engine/airport.h
    include/dxatc-engine/aircraft.h
    include/dxatc-engine/airline.h
    include/dxatc-engine/types.h
    include/dxatc-engine/callsign.h
    include/dxatc-engine/navdata.h
    include/dxatc-engine/world.h
    include/dxatc-engine/flightplan.h
    include/dxatc-engine/transponder.h
    include/dxatc-engine/frequency.h
)

set(dxatc-engine-src
    src/metar.c
)

add_library(dxatc-engine ${dxatc-engine-hdr} ${dxatc-engine-src})
target_link_libraries(dxatc-engine PRIVATE dxatc-utils)
