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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

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
)

foreach(SP_SOURCE
	src/soundManager.cpp
	src/clipboard.cpp
	src/postProcessManager.cpp
	src/fixedSocket.cpp
	src/input.cpp
	src/tween.cpp
	src/multiplayer_client.cpp
	src/PlayerController.cpp
	src/logging.cpp
	src/random.cpp
	src/gameEntity.cpp
	src/multiplayer_server_scanner.cpp
	src/networkRecorder.cpp
	src/multiplayer_server.cpp
	src/scriptInterfaceMagic.cpp
	src/Renderable.cpp
	src/Updatable.cpp
	src/textureManager.cpp
	src/collisionable.cpp
	src/stringImproved.cpp
	src/resources.cpp
	src/scriptInterface.cpp
	src/event.cpp
	src/multiplayer.cpp
	src/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
	src/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
	src/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
	src/Box2D/Dynamics/Contacts/b2CircleContact.cpp
	src/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
	src/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
	src/Box2D/Dynamics/Contacts/b2Contact.cpp
	src/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
	src/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
	src/Box2D/Dynamics/Joints/b2MouseJoint.cpp
	src/Box2D/Dynamics/Joints/b2MotorJoint.cpp
	src/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
	src/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
	src/Box2D/Dynamics/Joints/b2Joint.cpp
	src/Box2D/Dynamics/Joints/b2WheelJoint.cpp
	src/Box2D/Dynamics/Joints/b2RopeJoint.cpp
	src/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
	src/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
	src/Box2D/Dynamics/Joints/b2WeldJoint.cpp
	src/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
	src/Box2D/Dynamics/Joints/b2GearJoint.cpp
	src/Box2D/Dynamics/b2Fixture.cpp
	src/Box2D/Dynamics/b2World.cpp
	src/Box2D/Dynamics/b2Island.cpp
	src/Box2D/Dynamics/b2Body.cpp
	src/Box2D/Dynamics/b2ContactManager.cpp
	src/Box2D/Dynamics/b2WorldCallbacks.cpp
	src/Box2D/Rope/b2Rope.cpp
	src/Box2D/Collision/b2CollidePolygon.cpp
	src/Box2D/Collision/b2Distance.cpp
	src/Box2D/Collision/b2CollideCircle.cpp
	src/Box2D/Collision/Shapes/b2CircleShape.cpp
	src/Box2D/Collision/Shapes/b2ChainShape.cpp
	src/Box2D/Collision/Shapes/b2EdgeShape.cpp
	src/Box2D/Collision/Shapes/b2PolygonShape.cpp
	src/Box2D/Collision/b2TimeOfImpact.cpp
	src/Box2D/Collision/b2DynamicTree.cpp
	src/Box2D/Collision/b2BroadPhase.cpp
	src/Box2D/Collision/b2CollideEdge.cpp
	src/Box2D/Collision/b2Collision.cpp
	src/Box2D/Common/b2BlockAllocator.cpp
	src/Box2D/Common/b2StackAllocator.cpp
	src/Box2D/Common/b2Math.cpp
	src/Box2D/Common/b2Timer.cpp
	src/Box2D/Common/b2Draw.cpp
	src/Box2D/Common/b2Settings.cpp
	src/httpServer.cpp
	src/networkAudioStream.cpp
	src/engine.cpp
	src/windowManager.cpp
	src/shaderManager.cpp
	src/GL/glew.c
	src/lua/lundump.c
	src/lua/lmem.c
	src/lua/lctype.c
	src/lua/lparser.c
	src/lua/lmathlib.c
	src/lua/lbaselib.c
	src/lua/ltm.c
	src/lua/lstrlib.c
	src/lua/lfunc.c
	src/lua/ldo.c
	src/lua/lcorolib.c
	src/lua/lcode.c
	src/lua/lapi.c
	src/lua/lstate.c
	src/lua/llex.c
	src/lua/lopcodes.c
	src/lua/lobject.c
	src/lua/ldebug.c
	src/lua/lstring.c
	src/lua/ltable.c
	src/lua/lzio.c
	src/lua/ldblib.c
	src/lua/ldump.c
	src/lua/lvm.c
	src/lua/loadlib.c
	src/lua/lauxlib.c
	src/lua/ltablib.c
	src/lua/lgc.c
	src/lua/lbitlib.c
	)
  	list(APPEND SOURCES ${SERIOUS_PROTON_DIR}${SP_SOURCE})
endforeach()

add_definitions(-DWINDOW_TITLE="iMvaders")

add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${SERIOUS_PROTON_DIR}/src)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

## Setup SFML
find_package(SFML 2.3 REQUIRED system window graphics network audio)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

# Setup OpenGl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_EXECUTABLES ${EXECUTABLE_NAME})
set(CPACK_GENERATOR "ZIP")
include(CPack)
