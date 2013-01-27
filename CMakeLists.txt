cmake_minimum_required (VERSION 2.8.8)

project( propertystore )

SET( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" )

set(propertystore_MAJOR_VERSION 0)
set(propertystore_MINOR_VERSION 2)
set(propertystore_SUB_VERSION 0)
set(propertystore_VERSION ${propertystore_MAJOR_VERSION}.${propertystore_MINOR_VERSION}.${propertystore_SUB_VERSION})
set(VES_PACKAGE_VERSION ${propertystore_VERSION})

#Set the include directory so that propertystore can be used as a sub project
set(propertystore_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src" PARENT_SCOPE)
set(propertystore_LIBRARIES "propertystore" PARENT_SCOPE)

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

include(VESSetupCompiler)

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

set(CPACK_PACKAGE_VENDOR "Ames Lab")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library to assist in storing UI data.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt") 
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/README.txt") 
set(CPACK_PACKAGE_VERSION_MAJOR "${propertystore_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${propertystore_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${propertystore_SUB_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}_${VES_PACKAGE_VERSION}")
#This sits right below CPACK_PACKAGE_DEFAULT_LOCATION
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
#set(CPACK_TOPLEVEL_TAG "${PROJECT_NAME}")
#set(CPACK_SET_DESTDIR TRUE)
if(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
  set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
  set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
  set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
else(WIN32 AND NOT UNIX) 
  #This directory is the prefix to the default install prefix
  set(CPACK_PACKAGE_DEFAULT_LOCATION "/opt/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  set(CPACK_STRIP_FILES OFF)
  set(CPACK_SOURCE_STRIP_FILES OFF)
endif(WIN32 AND NOT UNIX)

include(CPack)
