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
#include <QDir>
#include <QFileInfo>

// Common headers.
#include "ErrorLogger.h"
#include "StringDeduplicator.h"

// Application headers.
#include "DataFileTracker.h"
#include "DataHierarchy.h"
#include "DataReader.h"
#include "DataWriter.h"

static DataFileTracker s_Files;

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
				s_Files.Add(fileName, id, hierarchy);
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

static bool WriteFile(const DataFileTracker::FileInfo& info)
{
	bool retval = false;
	DataWriter writer;
	QString outName = info.fileName + ".new";
	retval = writer.Write(info.hierarchy, outName, info.loaded);

	return retval;
}

static int TestApplyJournal()
{
	int retval = 0;

	// ../TestData/test.journal
	// ../TestData/players.data
	// ../TestData/layer.data
	// ../TestData/master.data

	SystemLogger.Fatal("Fatal Error test");
	SystemLogger.Verbose("Verbose debug test - shouldn't appear");
	SystemLogger.Log(ErrorLogger::NONFATAL_ERROR, "%s (%d)", "Non-fatal via vsprintf", 42);
	SystemLogger.Log((ErrorLogger::Severity)93, "Arbitrary severity");

	ErrorLogger::Severity sev = SystemLogger.Verbosity();
	SystemLogger.Verbosity(ErrorLogger::DEBUG);
	QString dmesg("Debug message via QString");
	SystemLogger.Debug(dmesg);
	SystemLogger.Verbosity(sev);
	
	ReadFile("../TestData/players.data");
	ReadFile("../TestData/master.data");
	ReadFile("../TestData/layer1000.data");

	DataFileTracker::FilesInfo files;
	s_Files.Files(files);

	for (int count = 0; count < files.count(); count++)
	{
		WriteFile(files[count]);
	}

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

	SystemLogger.Start("../Logs/ApplyJournal.log", "ApplyJournal v0.0");
	
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

	SystemLogger.Stop("ApplyJournal v0.0");

	return retval;
}
