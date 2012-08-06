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

# On windows provide the user with the pdb files 
# for debugging if they are present
if(MSVC)
    install(
        DIRECTORY ${PROJECT_BINARY_DIR}/lib/\${CMAKE_INSTALL_CONFIG_NAME}/
        DESTINATION lib
        USE_SOURCE_PERMISSIONS
        COMPONENT Development
        FILES_MATCHING PATTERN "*.pdb"
    )
endif(MSVC)

# Install the CMake find script for crunchstore
install( FILES "extra/Findpropertystore.cmake"
    DESTINATION "share/${CMAKE_PROJECT_NAME}/extra"
    COMPONENT Development
)

#
# Create and install all of the auto find tools
# =============================================

# Add all targets to the build-tree export set
export(TARGETS PropertySetBrowser
  FILE "${PROJECT_BINARY_DIR}/lib/propertystoreLibraryDepends.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE propertystore)

# Create a propertystoreBuildTreeSettings.cmake file for the use from the build tree
configure_file(propertystoreBuildTreeSettings.cmake.in
  "${PROJECT_BINARY_DIR}/lib/propertystoreBuildTreeSettings.cmake" @ONLY)

#configure_file(propertystoreConfig.cmake.in
#  "${PROJECT_BINARY_DIR}/lib/propertystoreConfig.cmake" @ONLY)
configure_file(propertystoreConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/lib/propertystoreConfigVersion.cmake" @ONLY)
configure_file(Usepropertystore.cmake.in
  "${PROJECT_BINARY_DIR}/lib/Usepropertystore.cmake" @ONLY)

# Install the export set for use with the install-tree
install(EXPORT propertystore-targets DESTINATION lib)

include( CMakePackageConfigHelpers )
# Create the propertystoreConfig.cmake and propertystoreConfigVersion files
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/lib"
   "${CMAKE_INSTALL_PREFIX}/include")

configure_package_config_file( 
    propertystoreConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/lib/propertystoreConfig.cmake
    INSTALL_DESTINATION lib
    PATH_VARS CONF_REL_INCLUDE_DIR )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/propertystoreConfig.cmake 
              ${CMAKE_CURRENT_BINARY_DIR}/lib/propertystoreConfigVersion.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/lib/Usepropertystore.cmake
        DESTINATION lib )

#
# Setup the fpc auto detection files
if(WIN32)
    set(propertystore_FPC_LIBRARIES "/libpath:\"\${libdir}\" PropertySetBrowser.lib")
    set(propertystore_FPC_INCLUDES "/I\"\${includedir}\"")
else(WIN32)
    set(propertystore_FPC_INCLUDES "-I\${includedir}")
    set(propertystore_FPC_LIBRARIES "-L\${libdir} -lPropertySetBrowser")
endif(WIN32)

configure_file(extra/propertystore.fpc.in
  "${PROJECT_BINARY_DIR}/lib/flagpoll/propertystore.fpc" @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/flagpoll/propertystore.fpc
        DESTINATION lib/flagpoll )
