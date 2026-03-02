set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

#set(HUNTER_PACKAGES CURL freetype giflib Jpeg PNG libogg vorbis ffmpeg OpenAL libxml2 Lua luabind OpenSSL ZLIB)
set(HUNTER_PACKAGES CURL freetype giflib Jpeg PNG libogg vorbis OpenAL libxml2 Lua luabind ZLIB)

set(HUNTER_ENABLED ON)
set(HUNTER_URL "https://github.com/cpp-pm/hunter/archive/v0.26.1.tar.gz")
set(HUNTER_SHA1 "e41ac7a18c49b35ebac99ff2b5244317b2638a65")
set(HUNTER_FILEPATH_CONFIG "${CMAKE_CURRENT_LIST_DIR}/../CMakeModules/HunterConfig.cmake")

include(FetchContent)
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)
