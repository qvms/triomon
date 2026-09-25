cmake_minimum_required(VERSION 3.22)
project(triomon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Xml
)

qt_standard_project_setup()

find_package(PkgConfig REQUIRED)

pkg_check_modules(PCAP REQUIRED IMPORTED_TARGET GLOBAL libpcap)
#-------------------------------------------------------------------------------------------------------------------------------------------------------
# DuckDB
# Default:
#   ${CMAKE_SOURCE_DIR}/bundle
#
# Override with:
#   -DDUCKDB_ROOT=/home/pwuid/Work/triomon/bundle
#-------------------------------------------------------------------------------------------------------------------------------------------------------

set(DUCKDB_ROOT
    "${CMAKE_SOURCE_DIR}/bundle"
    CACHE PATH
    "Root directory containing DuckDB files locally"
)

find_path(DUCKDB_INCLUDE_DIR
    NAMES duckdb.h
    PATHS
        "${DUCKDB_ROOT}/include"
        "${DUCKDB_ROOT}"
        "${DUCKDB_ROOT}/src/include"
    NO_DEFAULT_PATH
)

find_library(DUCKDB_LIBRARY
    NAMES
        duckdb
        libduckdb
        libduckdb_static
    PATHS
        "${DUCKDB_ROOT}"
        "${DUCKDB_ROOT}/lib"
    NO_DEFAULT_PATH
)

if(NOT DUCKDB_INCLUDE_DIR)
    message(FATAL_ERROR
        "Could not find a DuckDB library under:\n"
        "   ${DUCKDB_ROOT}\n\n"
        "Expected one of:\n"
        "   libduckdb.so\n"
        "   libduckdb_static.a\n"
    )
endif()

add_library(DuckDB::duckdb UNKNOWN IMPORTED GLOBAL)

set_target_properties(DuckDB::duckdb PROPERTIES
    IMPORTED_LOCATION "${DUCKDB_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${DUCKDB_INCLUDE_DIR}"
)

message(STATUS "DuckDB include directory: ${DUCKDB_INCLUDE_DIR}")
message(STATUS "DuckDB library:            ${DUCKDB_LIBRARY}")

qt_add_library(triomon
    PacketParser/Packets/AbstractPacket.cpp
    PacketParser/Packets/Ethernet2Packet.cpp
    PacketParser/Packets/RawPacket.cpp
    PacketParser/Frame.cpp
    PacketParser/PacketHandler.cpp
    PacketParser/NetworkHost.cpp
    PacketHandlerFramework/NetworkHostList.cpp
    PacketHandlerFramework/NetworkPacketList.cpp
    PacketParser/Packets/IPv4Packet.cpp
    PacketParser/Packets/TcpPacket.cpp
    PacketParser/Packets/UdpPacket.cpp
    PacketParser/Packets/IPv6Packet.cpp
    PacketParser/Packets/TpktPacket.cpp
    PacketParser/Packets/ArpPacket.cpp
    PacketParser/Packets/IcmpPacket.cpp
    PacketParser/Packets/Icmpv6Packet.cpp
    PacketParser/Packets/DnsPacket.cpp
    PacketParser/Packets/DhcpPacket.cpp
    PacketParser/Packets/TftpPacket.cpp
    PacketParser/Packets/NetBiosPacket.cpp
    PacketParser/Packets/NetBiosNameServicePacket.cpp
    PacketParser/Packets/NetBiosDatagramServicePacket.cpp
    PacketParser/Packets/NetBiosSessionService.cpp
    PacketParser/Packets/FtpPacket.cpp
    PacketParser/Packets/Pop3Packet.cpp
    PacketParser/Packets/SmtpPacket.cpp
    PacketParser/Packets/HttpPacket.cpp
    PacketParser/Packets/SyslogPacket.cpp
    PacketParser/Packets/ImapPacket.cpp
    PacketParser/Packets/IrcPacket.cpp
    PacketParser/Packets/NtlmSspPacket.cpp
    PacketParser/Packets/CifsBrowserPacket.cpp
    PacketParser/Packets/TabularDataStreamPacket.cpp
    PacketParser/Packets/IEEE_802_1Q_VlanPacket.cpp
    PacketParser/Packets/CiscoHdlcPacket.cpp
    PacketParser/Packets/PointToPointPacket.cpp
    PacketParser/Packets/PointToPointOverEthernetPacket.cpp
    PacketParser/Packets/LogicalLinkControlPacket.cpp
    PacketParser/Packets/HpSwitchProtocolPacket.cpp
    PacketParser/Packets/IEEE_802_11Packet.cpp
    PacketParser/Packets/CapwapPacket.cpp
    PacketParser/Packets/IEEE_802_11RadiotapPacket.cpp
    PacketParser/Packets/PrismCaptureHeaderPacket.cpp
    PacketParser/Packets/PpiPacket.cpp
    PacketParser/Packets/LinuxCookedCapture.cpp
    PacketParser/Packets/LinuxCookedCapture2.cpp
    PacketParser/Packets/NullLoopbackPacket.cpp
    PacketParser/Packets/ErfFrame.cpp
    PacketParser/Packets/TzspPacket.cpp
    PacketParser/Packets/Mpls.cpp
    PacketParser/Packets/Erspan.cpp
    PacketParser/Packets/PacketFactory.cpp
)

target_include_directories(triomon
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(triomon
    PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::Xml
    PRIVATE
        PkgConfig::PCAP
)

qt_add_executable(Triomon_App
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
    MainWindow.ui
    DatabaseManager.cpp
    DatabaseManager.hpp
)

target_link_libraries(Triomon_App
    PRIVATE
        triomon
        DuckDB::duckdb
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Xml
        PkgConfig::PCAP
)

include(CTest)

if(BUILD_TESTING)
    qt_add_executable(TestCli
        tests/test_cli.cpp
    )

    target_link_libraries(TestCli
        PRIVATE
            triomon
            Qt6::Core
            PkgConfig::PCAP
            Qt6::Network
    )

    add_test(
        NAME TestCli
        COMMAND TestCli
    )
endif()

include(GNUInstallDirs)

install(TARGETS Triomon_App
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(TARGET TestCli)
    install(TARGETS TestCli
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

install(TARGETS triomon
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
