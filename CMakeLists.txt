project(iMvaders)
cmake_minimum_required(VERSION 2.8.12)

# Check if serious proton dir is set.
if(NOT DEFINED SERIOUS_PROTON_DIR)
 message(FATAL_ERROR "SERIOUS_PROTON_DIR was not set. Unable to continue")
endif(NOT DEFINED SERIOUS_PROTON_DIR)

if(NOT DEFINED CPACK_PACKAGE_VERSION_MAJOR)
 string(TIMESTAMP CPACK_PACKAGE_VERSION_MAJOR "%Y")
 string(TIMESTAMP CPACK_PACKAGE_VERSION_MINOR "%m")
 string(TIMESTAMP CPACK_PACKAGE_VERSION_PATCH "%d")
endif(NOT DEFINED CPACK_PACKAGE_VERSION_MAJOR)

#set(OPT_FLAGS "-mcpu=native -mfpu=neon-vfpv4 -O3 -mfloat-abi=hard -funsafe-math-optimizations")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ${OPT_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ${OPT_FLAGS}")

## ensure c++11 is used
if(NOT ${CMAKE_VERSION} VERSION_LESS 3.1)
    set(CMAKE_CXX_STANDARD 11)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(EXECUTABLE_NAME "iMvaders")
set(SOURCES
	src/main.cpp
	src/BasicEnemyBase.cpp
	src/BasicEnemy.cpp
	src/bonusRound.cpp
	src/bre.cpp
	src/bullet.cpp
	src/BurstShotEnemy.cpp
	src/curve.cpp
	src/digitizer.cpp
	src/diveBomber.cpp
	src/EnemyGroup.cpp
	src/explosion.cpp
	src/gameState.cpp
	src/henk.cpp
	src/main.cpp
	src/mainMenu.cpp
	src/nuke.cpp
	src/objectListWatcher.cpp
	src/oldPrinters.cpp
	src/player.cpp
	src/powerupCarrier.cpp
	src/powerup.cpp
	src/replicatorZ18.cpp
	src/scoreManager.cpp
	src/StarBackground.cpp
	src/textDraw.cpp
	src/transmission.cpp
	src/versusGameMode.cpp
    src/gameEntity.cpp
    src/spriteManager.cpp
    src/PlayerController.cpp
)

add_subdirectory("${SERIOUS_PROTON_DIR}" "${PROJECT_BINARY_DIR}/SeriousProton" EXCLUDE_FROM_ALL)

add_definitions(-DWINDOW_TITLE="iMvaders")

add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} PUBLIC seriousproton)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_EXECUTABLES ${EXECUTABLE_NAME})
set(CPACK_GENERATOR "ZIP")
include(CPack)
