//
// StringDeduplicator.h
//
// Keep a single copy of all strings passed in and return a reference to that one copy.
//
// (c) 2014 Graham West

#include "StringDeduplicator.h"

StringDeduplicator::StringDeduplicator()
{
}

StringDeduplicator::~StringDeduplicator()
{
}

static QString emptyStr;

const QString& StringDeduplicator::Deduplicate(const QString& str)
{
	return emptyStr;
}
