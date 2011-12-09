SET( INCLUDE
     "${TARGET_INCLUDE_DIR}/Application.h"
     "${TARGET_INCLUDE_DIR}/Config.h"
     "${TARGET_INCLUDE_DIR}/Database.h"
     "${TARGET_INCLUDE_DIR}/global.h"
     "${TARGET_INCLUDE_DIR}/Log.h"
     "${TARGET_INCLUDE_DIR}/NetworkHandlers.h"
     "${TARGET_INCLUDE_DIR}/Session.h"
     "${TARGET_INCLUDE_DIR}/Singleton.h"
     "${TARGET_INCLUDE_DIR}/Storage.h"
     "${TARGET_INCLUDE_DIR}/Timer.h"

     "${TARGET_INCLUDE_DIR}/Packets/Opcodes.h"
     "${TARGET_INCLUDE_DIR}/Packets/SmartPacket.h"

     "${TARGET_INCLUDE_DIR}/Shared/Shared.h" )
SET( SOURCE
     "${TARGET_SOURCE_DIR}/Application.cpp"
     "${TARGET_SOURCE_DIR}/Config.cpp"
     "${TARGET_SOURCE_DIR}/Database.cpp"
     "${TARGET_SOURCE_DIR}/Log.cpp"
     "${TARGET_SOURCE_DIR}/main.cpp"
     "${TARGET_SOURCE_DIR}/NetworkHandlers.cpp"
     "${TARGET_SOURCE_DIR}/Session.cpp"
     "${TARGET_SOURCE_DIR}/Storage.cpp"
     "${TARGET_SOURCE_DIR}/Timer.cpp"

     "${TARGET_SOURCE_DIR}/Shared/Shared.cpp" )

########################
# Setup the executable #
########################
INCLUDE_DIRECTORIES( "${TARGET_INCLUDE_DIR}/.."
                     "${TARGET_INCLUDE_DIR}" )

SOURCE_GROUP( "include" FILES ${INCLUDE} )
SOURCE_GROUP( "src"     FILES ${SOURCE} )

ADD_EXECUTABLE( "${TARGET_NAME}"
                ${INCLUDE} ${SOURCE} )

TARGET_LINK_LIBRARIES( "${TARGET_NAME}"
                       ${MYSQL_LIBRARIES}
                       ${MYSQLPP_LIBRARIES}
                       ${Boost_LIBRARIES} )

IF( WIN32 )
    TARGET_LINK_LIBRARIES( "${TARGET_NAME}"
                           "wsock32" )
ENDIF( WIN32 )

SET( ${TARGET_NAME}_INCLUDE_DIR "${TARGET_INCLUDE_DIR}" PARENT_SCOPE )

INSTALL(TARGETS ServerFramework DESTINATION bin)
