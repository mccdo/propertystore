cmake_minimum_required (VERSION 2.8.8)

project( propertystore )

SET( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" )

set(propertystore_MAJOR_VERSION 0)
set(propertystore_MINOR_VERSION 2)
set(propertystore_SUB_VERSION 0)
set(propertystore_VERSION ${propertystore_MAJOR_VERSION}.${propertystore_MINOR_VERSION}.${propertystore_SUB_VERSION})

#option(BUILD_EXAMPLES "Build examples?" ON)

find_package( crunchstore 0.2.0 REQUIRED )
if( crunchstore_FOUND )
    include( ${crunchstore_USE_FILE} )
endif()

find_package( switchwire 0.2.0 REQUIRED )
if( switchwire_FOUND )
    include( ${switchwire_USE_FILE} )
endif()

SET(BOOST_INCLUDEDIR "$ENV{BOOST_INSTALL_DIR}/include" CACHE PATH "docstring")

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

FIND_PACKAGE(Boost 1.44.0 REQUIRED COMPONENTS system)

FIND_PACKAGE(POCO 1.4.1 REQUIRED COMPONENTS PocoFoundation PocoUtil PocoData PocoDataSQLite)

FIND_PACKAGE(Qt4 REQUIRED COMPONENTS QtCore QtGui )
include(${QT_USE_FILE})

INCLUDE_DIRECTORIES( 
    ${Boost_INCLUDE_DIRS}
    ${POCO_INCLUDE_DIR}
    ${switchwire_INCLUDE_DIRS}
    ${crunchstore_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/qtpropertybrowser-2.5_1-opensource/src
)

# Alias for the qtpropertybrowser path
SET( QTPBDIR ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser-2.5_1-opensource/src )

################################################################################
# Platform specific definitions
IF(WIN32)
    IF(MSVC)
        # To enable /MP, parralel build on MSVC
        OPTION(WIN32_USE_MP "Set to OFF to diable /MP." ON)
        MARK_AS_ADVANCED(WIN32_USE_MP)
        IF(WIN32_USE_MP)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        ENDIF(WIN32_USE_MP)

    ENDIF(MSVC)
ENDIF (WIN32)

IF(UNIX AND NOT WIN32 AND NOT APPLE)
  IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
      MARK_AS_ADVANCED(LIB_POSTFIX)
  ENDIF()
ENDIF()
IF(NOT DEFINED LIB_POSTFIX)
    SET(LIB_POSTFIX "")
ENDIF()

################################################################################
# Create lib directory if required

IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
ENDIF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

################################################################################

SET(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)
MAKE_DIRECTORY(${OUTPUT_LIBDIR})
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_LIBDIR})


add_subdirectory(src)

