#Lets us address our internal includes as <propertystore/foo.h>
include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QTPBDIR}
    )

# BasicTest
set( srcs BasicTest.cxx TestSet.cxx )

SET(TARGET_SRC ${srcs})

SET(TARGET_NAME BasicTest)
ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_SRC})
TARGET_LINK_LIBRARIES(${TARGET_NAME} propertystore ${QT_LIBRARIES})

SET(INSTALL_BINDIR bin)

INSTALL(
        TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${INSTALL_BINDIR} )

# LiveTest
set( srcs LiveTest.cxx LiveSet.cxx )

SET(TARGET_SRC ${srcs})

SET(TARGET_NAME LiveTest)
ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_SRC})
TARGET_LINK_LIBRARIES(${TARGET_NAME} propertystore ${QT_LIBRARIES})

SET(INSTALL_BINDIR bin)

INSTALL(
        TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${INSTALL_BINDIR}
    )

# LiveGUI
QT4_WRAP_CPP( MOCCED LiveGUI.h OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED )
QT4_WRAP_UI( UICED LiveGUI.ui OPTIONS )
set( srcs LiveGuiMain.cxx LiveGUI.cxx LiveSet.cxx )

SET(TARGET_SRC ${srcs})

SET(TARGET_NAME LiveGUI)
ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_SRC} ${MOCCED} ${UICED})
TARGET_LINK_LIBRARIES(${TARGET_NAME} propertystore ${QT_LIBRARIES})

SET(INSTALL_BINDIR bin)

INSTALL(
        TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${INSTALL_BINDIR}
    )

# UnitConversion
set( srcs UnitConversion.cxx )

SET(TARGET_SRC ${srcs})

SET(TARGET_NAME UnitConversion)
ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_SRC})
TARGET_LINK_LIBRARIES(${TARGET_NAME} propertystore ${QT_LIBRARIES})

SET(INSTALL_BINDIR bin)

INSTALL(
        TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${INSTALL_BINDIR}
    )

# DeclarativeProperties
#set( srcs DeclarativeProperties.cxx )
#
#SET(TARGET_SRC ${srcs})
#
#SET(TARGET_NAME DeclarativeProperties )
#ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_SRC})
#TARGET_LINK_LIBRARIES(${TARGET_NAME} propertystore ${QT_LIBRARIES})
#
#SET(INSTALL_BINDIR bin)
#
#INSTALL(
#        TARGETS ${TARGET_NAME}
#        RUNTIME DESTINATION ${INSTALL_BINDIR}
#    )
#
#install(FILES JSONProp.json
#    DESTINATION bin
#    COMPONENT development)
