#-------------------------------------------------
#
# Master qmake project file for CubeClicker
# Creates build files for the individual programs
#
#-------------------------------------------------

QT += core
QT -= gui

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

CONFIG += \
	ApplyJournal

DESTDIR = bin

# Code that belongs in all targets.
OBJECTS_DIR = common/build

HEADERS = \
	common/StringDeduplicator.h

SOURCES = \
	common/StringDeduplicator.cpp

ApplyJournal {
	TARGET = ApplyJournal

	OBJECTS_DIR = ApplyJournal/build

	HEADERS += \
		ApplyJournal/DataHierarchy.h \
		ApplyJournal/DataReader.h \
		ApplyJournal/DataWriter.h

	SOURCES += \
		ApplyJournal/main.cpp \
		ApplyJournal/DataHierarchy.cpp \
		ApplyJournal/DataReader.cpp \
		ApplyJournal/DataWriter.cpp
}

