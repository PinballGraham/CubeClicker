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
CONFIG += debug

CONFIG += \
	ApplyJournal

DESTDIR = bin

# Code that belongs in all targets.
OBJECTS_DIR = common/build

HEADERS = \
	common/ErrorLogger.h \
	common/StringDeduplicator.h \
	common/StringUtils.h

SOURCES = \
	common/ErrorLogger.cpp \
	common/StringDeduplicator.cpp \
	common/StringUtils.cpp

ApplyJournal {
	TARGET = ApplyJournal

	OBJECTS_DIR = ApplyJournal/build

	HEADERS += \
		ApplyJournal/DataFileTracker.h \
		ApplyJournal/DataHierarchy.h \
		ApplyJournal/DataReader.h \
		ApplyJournal/DataWriter.h \
		ApplyJournal/JournalParser.h

	SOURCES += \
		ApplyJournal/main.cpp \
		ApplyJournal/DataFileTracker.cpp \
		ApplyJournal/DataHierarchy.cpp \
		ApplyJournal/DataReader.cpp \
		ApplyJournal/DataWriter.cpp \
		ApplyJournal/JournalParser.cpp
}

