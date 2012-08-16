#Lets us address our internal includes as <propertystore/foo.h>
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${QTPBDIR}
        ${CMAKE_BINARY_DIR}/src/propertystore/__/qtpropertybrowser-2.5_1-opensource/src
    )

ADD_DEFINITIONS(-DPROPERTYSTORE_LIBRARY)

SET( QTPBDIR ${CMAKE_SOURCE_DIR}/src/qtpropertybrowser-2.5_1-opensource/src )

FILE( GLOB srcs "*.cxx" )
FILE( GLOB qsrcs "${QTPBDIR}/*.cpp" )
FILE( GLOB hdrs "*.h" )

SET( HDRS_TO_MOC
        ExternalStringSelect.h
        ExternalStringSelectFactory.h
        ExternalStringSelectManager.h
        FileEdit.h
        GenericPropertyBrowser.h
        PropertyParser.h
        ${QTPBDIR}/qtpropertybrowserutils_p.h
    )
QT4_WRAP_CPP( MOCCED ${HDRS_TO_MOC} )

SET( HDRS_TO_MOC_NO_COMPILE
        ${QTPBDIR}/qtbuttonpropertybrowser.h
        ${QTPBDIR}/qteditorfactory.h
        ${QTPBDIR}/qtgroupboxpropertybrowser.h
        ${QTPBDIR}/qtpropertybrowser.h
        ${QTPBDIR}/qtpropertymanager.h
        ${QTPBDIR}/qttreepropertybrowser.h
        ${QTPBDIR}/qtvariantproperty.h

)
FOREACH( _itr ${HDRS_TO_MOC_NO_COMPILE} )
    SET( _path ${CMAKE_BINARY_DIR}/src/propertystore/__/qtpropertybrowser-2.5_1-opensource/src )
    GET_FILENAME_COMPONENT( _fname ${_itr} NAME_WE)
    SET( _outFile moc_${_fname}.cxx )
    QT4_CREATE_MOC_COMMAND( ${_itr} ${_path}/${_outFile} "" "" )
    MACRO_ADD_FILE_DEPENDENCIES( ${QTPBDIR}/${_fname}.cpp ${_path}/${_outFile} )
ENDFOREACH( _itr ${HDRS_TO_MOC_NO_COMPILE} )


# Qt's internal projects sometimes contain moc-able information in source files
# rather than restricting to headers, and we have to handle this in a special
# way.
SET( SRCS_TO_MOC
        ${QTPBDIR}/qteditorfactory.cpp
        ${QTPBDIR}/qtpropertymanager.cpp
        ${QTPBDIR}/qttreepropertybrowser.cpp
    )
FOREACH( _itr ${SRCS_TO_MOC} )
    GET_FILENAME_COMPONENT( _fname ${_itr} NAME_WE)
    QT4_CREATE_MOC_COMMAND( ${_itr} "${CMAKE_BINARY_DIR}/src/propertystore/__/qtpropertybrowser-2.5_1-opensource/src/${_fname}.moc" "" "" )
    MACRO_ADD_FILE_DEPENDENCIES( ${_itr} ${CMAKE_BINARY_DIR}/src/propertystore/__/qtpropertybrowser-2.5_1-opensource/src/${_fname}.moc )
ENDFOREACH( _itr ${SRCS_TO_MOC} )
### End special stuff for sources with moc info

SET( QRC_FILES ${QTPBDIR}/qtpropertybrowser.qrc )
QT4_ADD_RESOURCES( RCCED ${QRC_FILES} )

SET(TARGET_SRC ${srcs} ${qsrcs} ${RCCED})
SET(PUBLIC_HEADERS ${hdrs})

set(VES_PRIVATE_HEADERS ${hdrs})
set(VES_PUBLIC_HEADERS ${hdrs} )

set(VES_TARGET_NAME propertystore)
set(VES_TARGET_VERSION ${propertystore_VERSION})
set(VES_TARGET_EXPORT ${CMAKE_PROJECT_NAME}-targets)
set(VES_TARGET_CATEGORY Lib)

ADD_LIBRARY(${VES_TARGET_NAME} SHARED ${TARGET_SRC} ${VES_PUBLIC_HEADERS} ${MOCCED} ${RCCED})
TARGET_LINK_LIBRARIES(${VES_TARGET_NAME} ${switchwire_LIBRARIES} ${crunchstore_LIBRARIES} ${POCO_LIBRARIES} ${QT_LIBRARIES}) 

include(VESModuleInstall)
