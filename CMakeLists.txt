# vim: set ts=4 sw=4 sts=4 et:

PROJECT (leveldb)
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

SET (LEVELDB_MAJOR 1)
SET (LEVELDB_MINOR 18)

OPTION (ENABLE_SO_VERSION "Enable shared library version" ON)
OPTION (LIB_SUFFIX "Installation library directory suffix")
OPTION (LEVELDB_VERSION "Set leveldb version")

IF (LEVELDB_VERSION)
    IF (LEVELDB_VERSION MATCHES "([0-9]+)\\.([0-9]+)")
        SET (LEVELDB_MAJOR ${CMAKE_MATCH_1})
        SET (LEVELDB_MINOR ${CMAKE_MATCH_2})
    ENDIF (LEVELDB_VERSION MATCHES "([0-9]+)\\.([0-9]+)")
ENDIF (LEVELDB_VERSION)

IF (LIB_SUFFIX)
    SET (INSTALL_LIBDIR lib${LIB_SUFFIX})
ELSEIF (LIB_SUFFIX STREQUAL "" OR NOT CMAKE_VERSION OR CMAKE_VERSION VERSION_LESS "2.8.5")
    SET (INSTALL_LIBDIR lib)
ELSE (LIB_SUFFIX STREQUAL "" OR NOT CMAKE_VERSION OR CMAKE_VERSION VERSION_LESS "2.8.5")
    INCLUDE (GNUInstallDirs)
    SET(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
ENDIF(LIB_SUFFIX)

INCLUDE (CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX ("stdint.h" HAVE_STDINT_H)
IF (HAVE_STDINT_H)
    ADD_DEFINITIONS(-DHAVE_STDINT_H=1)
ENDIF (HAVE_STDINT_H)

CHECK_INCLUDE_FILE_CXX ("inttypes.h" HAVE_INTTYPES_H)
IF (HAVE_INTTYPES_H)
    ADD_DEFINITIONS(-DHAVE_INTTYPES_H=1)
ENDIF (HAVE_INTTYPES_H)

CHECK_INCLUDE_FILE_CXX ("endian.h" HAVE_ENDIAN_H)
IF (HAVE_ENDIAN_H)
    ADD_DEFINITIONS(-DHAVE_ENDIAN_H=1)
ENDIF (HAVE_ENDIAN_H)

INCLUDE (TestBigEndian)
TEST_BIG_ENDIAN(ENDIAN)
IF (ENDIAN)
    ADD_DEFINITIONS (-DCMAKE_BIG_ENDIAN=1)
ENDIF (ENDIAN)

INCLUDE (CheckCXXCompilerFlag)

# If -std=c++0x works, use <atomic> as fallback for when memory barriers
# are not available.
CHECK_CXX_COMPILER_FLAG (-std=c++0x HAVE_STD_CXX0X)
IF (HAVE_STD_CXX0X)
    ADD_DEFINITIONS (-std=c++0x)

    INCLUDE (CMakePushCheckState)

    CMAKE_PUSH_CHECK_STATE (RESET)
    SET (CMAKE_REQUIRED_FLAGS -std=c++0x)
    CHECK_INCLUDE_FILE_CXX ("atomic" HAVE_ATOMIC)
    CMAKE_POP_CHECK_STATE ()
    IF (HAVE_ATOMIC)
        ADD_DEFINITIONS (-DLEVELDB_ATOMIC_PRESENT)
    ENDIF (HAVE_ATOMIC)
ENDIF (HAVE_STD_CXX0X)

CHECK_CXX_COMPILER_FLAG (-Wall HAVE_WALL)
IF (HAVE_WALL)
    ADD_DEFINITIONS (-Wall)
ENDIF (HAVE_WALL)

IF (UNIX)
    IF (HAVE_ATOMIC)
        ADD_DEFINITIONS (-DLEVELDB_PLATFORM_POSIX)
    ENDIF (HAVE_ATOMIC)

    CHECK_CXX_COMPILER_FLAG (-pthread HAVE_PTHREAD)
    IF (HAVE_PTHREAD)
        ADD_DEFINITIONS (-pthread)
    ENDIF (HAVE_PTHREAD)

    # Use libc's memcmp instead of GCC's memcmp.  This results in ~40%
    # performance improvement on readrandom under gcc 4.4.3 on Linux/x86.
    CHECK_CXX_COMPILER_FLAG (-fno-builtin-memcmp HAVE_FNO_BUILTIN_MEMCMP)
    IF (HAVE_FNO_BUILTIN_MEMCMP)
        ADD_DEFINITIONS (-fno-builtin-memcmp)
    ENDIF (HAVE_FNO_BUILTIN_MEMCMP)

    IF ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        ADD_DEFINITIONS (-DOS_LINUX)
    ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
        ADD_DEFINITIONS (-D_REENTRANT -DOS_SOLARIS)
    ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
        ADD_DEFINITIONS (-D_REENTRANT -DOS_FREEBSD)
    ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "NetBSD")
        ADD_DEFINITIONS (-D_REENTRANT -DOS_NETBSD)
    ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD")
        ADD_DEFINITIONS (-D_REENTRANT -DOS_OPENBSD)
    ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "DragonFly")
        ADD_DEFINITIONS (-D_REENTRANT -DOS_DRAGONFLYBSD)
    ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "HP-UX")
        ADD_DEFINITIONS (-D_REENTRANT -DOS_HPUX)
    ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "AIX")
        ADD_DEFINITIONS (-D_REENTRANT -DOS_AIX)
    ENDIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

    SET (LEVELDB_PORT_SOURCES
        port/port_posix.cc
        util/env_posix.cc)
ELSEIF (WIN32)
    IF (CYGWIN)
        ADD_DEFINITIONS (-D_REENTRANT -DOS_LINUX -DCYGWIN)
        SET (LEVELDB_PORT_SOURCES
            port/port_posix.cc
            util/env_posix.cc)
    ELSEIF (MINGW)
        ADD_DEFINITIONS (
            -D_REENTRANT
            -DOS_WINDOWS
            -DLEVELDB_PLATFORM_WINDOWS
            -D_WIN32_WINNT=0x0501
            -DWIN32)
        SET (LEVELDB_PORT_SOURCES
            port/port_win.cc
            util/env_boost.cc
            util/win_logger.cc)
    ENDIF (CYGWIN)
ELSEIF (APPLE)
    IF ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        ADD_DEFINITIONS(-DOS_MACOSX)
        SET (LEVELDB_PORT_SOURCES port/port_posix.cc)
    ENDIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
ENDIF (UNIX)

INCLUDE_DIRECTORIES (
    include
    .
)

SET (LEVELDB_SOURCES
    db/builder.cc
    db/c.cc
    db/dbformat.cc
    db/db_impl.cc
    db/db_iter.cc
    db/dumpfile.cc
    db/filename.cc
    db/log_reader.cc
    db/log_writer.cc
    db/memtable.cc
    db/repair.cc
    db/table_cache.cc
    db/version_edit.cc
    db/version_set.cc
    db/write_batch.cc
    table/block_builder.cc
    table/block.cc
    table/filter_block.cc
    table/format.cc
    table/iterator.cc
    table/merger.cc
    table/table_builder.cc
    table/table.cc
    table/two_level_iterator.cc
    util/arena.cc
    util/bloom.cc
    util/cache.cc
    util/coding.cc
    util/comparator.cc
    util/crc32c.cc
    util/env.cc
    util/filter_policy.cc
    util/hash.cc
    util/histogram.cc
    util/logging.cc
    util/options.cc
    util/status.cc
)

ADD_LIBRARY (leveldb SHARED ${LEVELDB_SOURCES} ${LEVELDB_PORT_SOURCES})

IF (WIN32 AND MINGW)
    FIND_PACKAGE (Boost REQUIRED COMPONENTS
        filesystem
        system
        chrono
        thread_win32)
    INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES (leveldb ${Boost_LIBRARIES})
ENDIF (WIN32 AND MINGW)

IF (ENABLE_SO_VERSION)
    SET_TARGET_PROPERTIES (leveldb PROPERTIES
        SOVERSION ${LEVELDB_MAJOR}
        VERSION ${LEVELDB_MAJOR}.${LEVELDB_MINOR})
ENDIF (ENABLE_SO_VERSION)

INSTALL (TARGETS leveldb
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    ARCHIVE DESTINATION lib${LIB_SUFFIX})

INSTALL (DIRECTORY include/leveldb DESTINATION include)
