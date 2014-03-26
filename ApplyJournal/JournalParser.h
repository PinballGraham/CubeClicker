//
// JournalParser.h
//
// Read a journal file and apply updates to the data files, checking each
// transaction's integrity.
//
// (c) 2014 Graham West

#if !defined(JOURNALPARSER_H)
#define JOURNALPARSER_H

// Library headers.
#include <QString>

class JournalParser
{
public:
	explicit JournalParser(const QString& fileName, bool fixChecksums = false);
	~JournalParser();

	bool Process();

private:
	JournalParser();
	JournalParser(const JournalParser& src);
	JournalParser& operator=(const JournalParser& src);

	QString m_fileName;
	bool m_fixChecksums;
};

#endif // JOURNALPARSER_H