//
// JournalParser.cpp
//
// Read a journal file and apply updates to the data files, checking each
// transaction's integrity.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "JournalParser.h"

// Library headers.
#include <QFile>

// Common headers.
#include "StringDeduplicator.h"

JournalParser::JournalParser(const QString& fileName, bool fixChecksums) :
	m_fileName(fileName), m_fixChecksums(fixChecksums)
{
}

JournalParser::~JournalParser()
{
}

bool JournalParser::Process()
{
	bool retval = false;

	return retval;
}
