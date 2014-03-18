//
// main.cpp
//
// Read, process, write and replace data files after applying entries from a
// journal file. The journal and data file names are parsed from command line
// arguments.
//
// (c) 2014 Graham West

// System headers.
#include <stdio.h>

// Library headers.
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QMap>
#include <QVector>

// Common headers.
#include "StringDeduplicator.h"

// Application headers.
#include "DataHierarchy.h"
#include "DataReader.h"
#include "DataWriter.h"

typedef struct {
	QString fileName;
	DataHierarchy* hierarchy;
	QDateTime loaded;
} MasterEntry;

typedef QMap<QString, MasterEntry> MasterMap;

static MasterMap s_root;

static QString FullFileName(const QString& relativeName)
{
	QFileInfo info(QDir::current(), relativeName);
	return info.canonicalFilePath();
}

static bool ReadFile(const QString& fileName)
{
	bool retval = false;
	QString fullName = FullFileName(fileName);

	if (!fullName.isEmpty())
	{
		DataReader reader;
		DataHierarchy* hierarchy;
		
		hierarchy = reader.Read(fullName);

		if (hierarchy)
		{
			DataValue idVal = hierarchy->Value("id");

			if (idVal.IsBasic())
			{
				QString id = idVal.BasicString();

				MasterEntry newEntry;
				newEntry.fileName = fileName;
				newEntry.hierarchy = hierarchy;
				newEntry.loaded = QDateTime::currentDateTime();

				s_root.insert(id, newEntry);
			}
			else
			{
				// XXX: Report id-less file.
				delete hierarchy;
			}
		}
		else
		{
			// XXX: Report unreadable file.
		}
	}
	else
	{
		// XXX: Report missing file.
	}

	return retval;
}

static int TestApplyJournal()
{
	int retval = 0;

	// ../TestData/test.journal
	// ../TestData/players.data
	// ../TestData/layer.data
	// ../TestData/master.data

	ReadFile("../TestData/players.data");
	return retval;
}

static int ApplyJournal(int argc, char* argv[])
{
	int retval = 0;

	if (argc < 3)
	{
		retval = 1;
	}
	else
	{
		QString journalName(argv[1]);
		// Vector of file IDs
		for (int count = 2; count < argc; count++)
		{
			QString fileName(argv[count]);
			// Read file by name, get ID
		}
		// Modify files with journal
		for (int count = 2; count < argc; count++)
		{
			QString fileName(argv[count]);
			// Write data for ID to .new file
		}

		// Rename journal file to .processed
		// Rename current files to .old and .new to current.
		// Delete the .old and .processed files.
	}
	
	return retval;
}

int main(int argc, char *argv[])
{
	int retval = 0;
	bool testMode = false;

	if (argc == 2)
	{
		QString param(argv[1]);

		if (param.compare("-test", Qt::CaseInsensitive) == 0)
		{
			testMode = true;
		}
	}

	if (testMode)
	{
		retval = TestApplyJournal();
	}
	else if (argc < 3)
	{
		printf("%s: <journal file> <data file> [data file] ...\n", argv[0]);
		retval = 1;
	}
	else
	{
		retval = ApplyJournal(argc, argv);
	}
	
	return retval;
}
