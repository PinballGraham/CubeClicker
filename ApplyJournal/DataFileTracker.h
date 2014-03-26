//
// DataFileTracker.h
//
// Track filenames and IDs of loaded data files, along with their hierarchy.
//
// (c) 2014 Graham West

#if !defined(DATAFILETRACKER_H)
#define DATAFILETRACKER_H

// Library includes.
#include <QDateTime>
#include <QMap>
#include <QString>
#include <QVector>

// Application includes.
#include "DataHierarchy.h"

class DataFileTracker
{
public:
	typedef struct FileInfo {
		QString fileName;
		DataHierarchy* hierarchy;
		QDateTime loaded;
	} FileInfo;

	typedef QVector<FileInfo> FilesInfo;

	DataFileTracker();
	~DataFileTracker();

	bool Add(const QString& fileName, const QString& id, DataHierarchy* hierarchy);

	DataHierarchy* Hierarchy(const QString& id);
	void Files(FilesInfo& filesDest);

private:
	DataFileTracker(const DataFileTracker& src);
	DataFileTracker& operator=(const DataFileTracker& src);

	typedef QMap<QString, FileInfo> FilesMap;

	FilesMap m_Files;
};

#endif // DATAFILETRACKER_H