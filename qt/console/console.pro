TEMPLATE = app
QT       += widgets network

HEADERS  += \
    ../pubnub_qt.h\
    pubnubconsole.h

SOURCES += \
    ../../core/pubnub_ccore.c\
    ../../core/pubnub_assert_std.c\
    ../../core/pubnub_json_parse.c\
    ../../core/pubnub_helper.c\
    ../pubnub_qt.cpp\
    pubnubconsole.cpp\
    main.cpp

win32:SOURCES += ../../core/c99/snprintf.c

INCLUDEPATH += ../
INCLUDEPATH += ../../core
win32:INCLUDEPATH += ../../core/c99

DEPENDPATH += ../
DEPENDPATH += ../../core

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/pubnub_qt
INSTALLS += target

FORMS    += pubnubconsole.ui
