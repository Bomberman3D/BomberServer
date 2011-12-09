# - Find MySQL++
# Find the native MySQL++ includes and library
#
#   MYSQLPP_FOUND       - True if MySQL++ found.
#   MYSQLPP_INCLUDE_DIR - where to find mysql++.h, etc.
#   MYSQLPP_LIBRARIES   - List of libraries when using MySQL++.
#

IF( MYSQLPP_INCLUDE_DIR )
    # Already in cache, be silent
    SET( MySQL++_FIND_QUIETLY TRUE )
ENDIF( MYSQLPP_INCLUDE_DIR )

FIND_PATH( MYSQLPP_INCLUDE_DIR "mysql++.h"
           PATH_SUFFIXES "mysql++" )

FIND_LIBRARY( MYSQLPP_LIBRARIES
              NAMES "mysqlpp"
              PATH_SUFFIXES "mysql++" )

# handle the QUIETLY and REQUIRED arguments and set MYSQLPP_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( "MySQL++" DEFAULT_MSG MYSQLPP_INCLUDE_DIR MYSQLPP_LIBRARIES )

MARK_AS_ADVANCED( MYSQLPP_INCLUDE_DIR MYSQLPP_LIBRARIES )
