#ifndef __DIRECTORYLISTING_H__
#define __DIRECTORYLISTING_H__

class CDirentry
{
public:
	wxString name;
	wxLongLong size;
	wxString permissions;
	wxString ownerGroup;
	bool dir;
	bool link;
	wxString target; // Set to linktarget it link is true

	bool hasDate;
	bool hasTime;
	wxDateTime time;

	bool unsure; // May be true on cached items if any changes were made to the file

	wxString dump() const;
	bool operator==(const CDirentry &op) const;
};

#define UNSURE_ADD		0x01
#define UNSURE_REMOVE	0x02
#define UNSURE_CHANGE	0x04
#define UNSURE_CONFUSED	0x05

class CDirectoryListing
{
public:
	CDirectoryListing();
	~CDirectoryListing();

	unsigned int m_entryCount;

	CServerPath path;
	CDirentry *m_pEntries;
	CDirectoryListing& operator=(const CDirectoryListing &a);

	// Lowest bit indicates a file got added
	// Next bit indicates a file got removed
	// 3rd bit indicates a file got changed.
	// 4th bit is set if an update cannot be applied to
	// one of the other categories.
	// 
	// These bits should help the user interface to choose an appropriate sorting
	// algorithm for modified listings
	int m_hasUnsureEntries;
	bool m_failed;

	CTimeEx m_firstListTime;
};

#endif

