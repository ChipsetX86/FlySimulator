QT += core qml quick

CONFIG += c++17 qmltypes

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        AppEngine.cpp \
        Mucha.cpp \
        PlotModel.cpp \
        main.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    FlySimulator_ru_RU.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AppEngine.h \
    Mucha.h \
    PlotModel.h

QML_IMPORT_NAME = io.qt.backend
QML_IMPORT_MAJOR_VERSION = 1

DISTFILES += \
    mucha.svg
