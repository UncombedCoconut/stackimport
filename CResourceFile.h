/*
 *  CResourceFile.h
 *  stackimport
 *
 *  Created by Justin Blanchard on 11/11/15.
 *  License: MIT
 *
 */


// If you're not compiling for a platform where the Mac resource manager is
//	available, set the following to 0 to remove that code from compilation:
#define MAC_CODE		0

// If you're compiling for 64 bit, you don't have access to QuickTime, which
//	we use to create AIFF files from 'snd ' resources. So turn this off.
#define USE_QUICKTIME	(!__LP64__)

#include <string>
#include <stdint.h>
#include <stdlib.h>

#if MAC_CODE
#include <Carbon/Carbon.h>
#if USE_QUICKTIME
#include <QuickTime/QuickTime.h>
#endif
#endif


class CResource
{
public:
	CResource();
	operator bool();
	int16_t				GetID();
	const std::string&	GetName();
	const char*			GetBuffer();
	size_t				GetSize();
#if MAC_CODE
	CResource( Handle handle );
	Handle				GetHandle();
#else
#endif
protected:
	int16_t		mID;
	std::string	mName;
#if MAC_CODE
	Handle		mHandle;
#else
#endif
};


class CResourceFile
{
public:
	CResourceFile();
	void		LoadFile( const std::string& fpath );
	void		Close();
	int16_t		Count( const std::string& type );
	CResource	GetByID( const std::string& type, int16_t id );
	CResource	GetByIndex( const std::string& type, int16_t index );
protected:
#if MAC_CODE
	OSType		MakeOSType( const std::string& type );

	SInt16		mResRefNum;
#endif
};
