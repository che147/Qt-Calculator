QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG  += c++17

TARGET   = Calculator
TEMPLATE = app

# 编译产物统一放到 build 目录，保持源码目录干净
OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
UI_DIR      = build/ui

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui
