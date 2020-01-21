QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../src/closingdockwidget.cpp \
    ../src/dbif.cpp \
    ../src/libcore.cpp \
    ../src/libviewwidget.cpp \
    ../src/libwindow.cpp \
    ../src/loggable.cpp \
    ../src/logger.cpp \
    ../src/main.cpp \
    ../src/uimanager.cpp \
    ../src/filesaveas.cpp \
    ../src/utils.cpp


HEADERS += \
    ../src/closingdockwidget.h \
    ../src/connable.h \
    ../src/coreable.h \
    ../src/dbif.h \
    ../src/idbif.h \
    ../src/ilogger.h \
    ../src/iuimanager.h \
    ../src/libcore.h \
    ../src/libviewwidget.h \
    ../src/libwindow.h \
    ../src/loggable.h \
    ../src/logger.h \
    ../src/uimanager.h \
    ../src/filesaveas.h


FORMS += \
    ../src/ui/libwindow.ui \

RESOURCES += \
    ../src/application.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

