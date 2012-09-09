#Lets us address our internal includes as <propertystore/foo.h>
include_directories(
        ${CMAKE_SOURCE_DIR}/src
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
        RUNTIME DESTINATION ${INSTALL_BINDIR}
    )

# LiveTest
set( srcs LiveTest.cxx )

SET(TARGET_SRC ${srcs})

SET(TARGET_NAME LiveTest)
ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_SRC})
TARGET_LINK_LIBRARIES(${TARGET_NAME} propertystore ${QT_LIBRARIES})

SET(INSTALL_BINDIR bin)

INSTALL(
        TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${INSTALL_BINDIR}
    )
