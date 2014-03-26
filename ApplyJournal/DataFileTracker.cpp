//
// DataFileTracker.h
//
// Track filenames and IDs of loaded data files, along with their hierarchy.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "DataFileTracker.h"

// Library includes.

// Common includes.
#include "StringDeduplicator.h"

DataFileTracker::DataFileTracker()
{
}

DataFileTracker::~DataFileTracker()
{
}

bool DataFileTracker::Add(const QString& fileName, const QString& id, DataHierarchy* hierarchy)
{
	bool retval = false;

	if (!m_Files.contains(id))
	{
		FileInfo newFile;
		newFile.fileName = fileName;
		newFile.hierarchy = hierarchy;
		newFile.loaded = QDateTime::currentDateTime();

		m_Files.insert(id, newFile);
		retval = true;
	}

	return retval;
}

DataHierarchy* DataFileTracker::Hierarchy(const QString& id)
{
	DataHierarchy* retval = 0;

	FilesMap::const_iterator iter = m_Files.find(id);

	if (iter != m_Files.end())
	{
		retval = iter->hierarchy;
	}

	return retval;
}

void DataFileTracker::Files(DataFileTracker::FilesInfo& filesDest)
{
	filesDest.clear();

	FilesMap::const_iterator iter = m_Files.begin();

	while (iter != m_Files.end())
	{
		filesDest.push_back(*iter);
		iter++;
	}
}
