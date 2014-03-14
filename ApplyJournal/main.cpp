//
// main.cpp
//
// Read, process, write and replace data files after applying entries from a
// journal file. The journal and data file names are parsed from command line
// arguments.
//
// (c) 2014 Graham West

#define TEST_FILENAMES

// System headers.
#include <stdio.h>

// Library headers.
#include <QHash>
#include <QMap>
#include <QVector>

// Common headers.
#include "StringDeduplicator.h"

// Application headers.
#include "DataHierarchy.h"

#if defined(TEST_FILENAMES)
void TestApplyJournal()
{
	QVector<QString> fileIDs;
	// ../TestData/test.journal
	// ../TestData/players.data
	// ../TestData/layer.data
	// ../TestData/master.data
}
#endif

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
			printf("%d: %s\n", count - 1, fileName.toLocal8Bit().data());
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
	
#if defined(TEST_FILENAMES)
	TestApplyJournal();
#else
	if (argc < 3)
	{
		printf("%s: <journal file> <data file> [data file] ...\n", argv[0]);
		retval = 1;
	}
	else
	{
		retval = ApplyJournal(argc, argv);
	}
#endif
	
	return retval;
}
