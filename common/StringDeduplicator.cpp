//
// StringDeduplicator.h
//
// Keep a single copy of all strings passed in and return a reference to that
// one copy.
//
// (c) 2014 Graham West

// Class header, always comes first.
#include "StringDeduplicator.h"

// Library includes.
#include <QHash>

// Used when we're asked to retrieve something that hasn't been stored.
static const QString emptyStr("");

StringDeduplicator* StringDeduplicator::m_Instance = 0;

StringDeduplicator::StringDeduplicator()
{
}

StringDeduplicator::~StringDeduplicator()
{
}

StringDeduplicator* StringDeduplicator::Instance()
{
	if (!m_Instance)
	{
		m_Instance = new StringDeduplicator;
	}
	
	return m_Instance;
}

uint StringDeduplicator::Find(const QString& str)
{
	uint retval = 0;
	StringDeduplicator* dedup = StringDeduplicator::Instance();
	uint strHash = qHash(str);

	if (dedup->m_Strings.contains(strHash))
	{
		retval = strHash;
	}

	return retval;
}

uint StringDeduplicator::Store(const QString& str)
{
	StringDeduplicator* dedup = StringDeduplicator::Instance();
	DeduplicatorMap::const_iterator iter = dedup->m_Strings.end();
	
	uint strHash = qHash(str);
	iter = dedup->m_Strings.insert(strHash, str);
	
	return iter.key();
}

const QString& StringDeduplicator::Retrieve(uint hash)
{
	StringDeduplicator* dedup = StringDeduplicator::Instance();
	DeduplicatorMap::const_iterator iter = dedup->m_Strings.find(hash);
	
	if (iter == dedup->m_Strings.end())
	{
		return emptyStr;
	}
	else
	{
		return iter.value();
	}
}

int StringDeduplicator::Total()
{
	int retval = 0;
	StringDeduplicator* dedup = StringDeduplicator::Instance();
	
	retval = dedup->m_Strings.size();
	return retval;
}
