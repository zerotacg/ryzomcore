set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

set(HUNTER_PACKAGES CURL freetype giflib Jpeg PNG libogg vorbis OpenAL libxml2 Lua luabind ZLIB)

# cross compiling doesn't quite work on the hunter openssl package therefor compile it separately and use it
IF(NOT CMAKE_CROSSCOMPILING)
    list(APPEND HUNTER_PACKAGES OpenSSL)
ENDIF()
IF(WITH_FFMPEG)
    list(APPEND HUNTER_PACKAGES ffmpeg)
ENDIF()

set(HUNTER_ENABLED ON)
set(HUNTER_URL "https://github.com/cpp-pm/hunter/archive/v0.26.1.tar.gz")
set(HUNTER_SHA1 "e41ac7a18c49b35ebac99ff2b5244317b2638a65")
set(HUNTER_FILEPATH_CONFIG "${CMAKE_CURRENT_LIST_DIR}/../CMakeModules/HunterConfig.cmake")

include(FetchContent)
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)
