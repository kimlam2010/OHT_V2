# OHT-50 Firmware Dependencies
# Version: 1.0.0

# Required packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Optional packages
if(ENABLE_LIDAR)
    # LiDAR SDK (if available)
    # find_package(RPLidar QUIET)
endif()

# System libraries
set(SYSTEM_LIBS
    pthread
    m
    dl
    rt
)

# SSL/Crypto libraries
set(SSL_LIBS
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Check for required headers
include(CheckIncludeFile)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file("netinet/in.h" HAVE_NETINET_IN_H)
check_include_file("arpa/inet.h" HAVE_ARPA_INET_H)
check_include_file("pthread.h" HAVE_PTHREAD_H)

# Check for required functions
include(CheckFunctionExists)
check_function_exists("clock_gettime" HAVE_CLOCK_GETTIME)
check_function_exists("pthread_create" HAVE_PTHREAD_CREATE)

# Report dependencies
message(STATUS "Dependencies:")
message(STATUS "  Threads: ${CMAKE_THREAD_LIBS_INIT}")
message(STATUS "  OpenSSL: ${OPENSSL_VERSION}")
message(STATUS "  OpenSSL Include: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "  OpenSSL Libraries: ${OPENSSL_LIBRARIES}")

# Export dependency information
set(ALL_SYSTEM_LIBS ${SYSTEM_LIBS} ${SSL_LIBS} ${CMAKE_THREAD_LIBS_INIT})


