cmake_minimum_required (VERSION 2.8.8)

project( propertystore )

SET( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" )

set(propertystore_MAJOR_VERSION 0)
set(propertystore_MINOR_VERSION 2)
set(propertystore_SUB_VERSION 0)
set(propertystore_VERSION ${propertystore_MAJOR_VERSION}.${propertystore_MINOR_VERSION}.${propertystore_SUB_VERSION})
set(VES_PACKAGE_VERSION ${propertystore_VERSION})

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

set( POCO_ROOT "Please specify" CACHE PATH "Root directory where POCO is installed" )
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

include(VESBinAndLibDirs)

add_subdirectory(src)

#
# Doxygen
#find_package( Doxygen )
#if( DOXYGEN_FOUND )
#    set( HAVE_DOT "NO" )
#    if( DOXYGEN_DOT_PATH )
#        set( HAVE_DOT "YES" )
#    endif()
#    
#    configure_file( ${PROJECT_SOURCE_DIR}/doc/doxyfile.cmake
#        ${PROJECT_BINARY_DIR}/doc/doxyfile
#    )
#    add_custom_target( Documentation ${DOXYGEN_EXECUTABLE}
#        ${PROJECT_BINARY_DIR}/doc/doxyfile
#    )
#    
#    add_subdirectory( doc/images )
#endif()

include(VESInstallPDBFiles)

set(VES_EXPORT_LIBRARY_TARGETS propertystore)
set(VES_CONFIGURE_VARS CMAKE_INSTALL_LIBDIR)

#
# Setup the fpc auto detection files
if(WIN32)
    set(propertystore_FPC_LIBRARIES "/libpath:\"\${libdir}\" propertystore.lib")
    set(propertystore_FPC_INCLUDES "/DQT_QTPROPERTYBROWSER_IMPORT /I\"\${includedir}\" /I\"\${includedir}/qtpropertybrowser\"")
else(WIN32)
    set(propertystore_FPC_INCLUDES "-I\${includedir} -I\${includedir}/qtpropertybrowser")
    set(propertystore_FPC_LIBRARIES "-L\${libdir} -lpropertystore")
endif(WIN32)

include(VESInstallAutoDiscoveryTools)
