/*
 *  CResourceFile.cpp
 *  stackimport
 *
 *  Created by Justin Blanchard on 11/11/15.
 *  License: MIT
 *
 */


#include "CResourceFile.h"


CResourceFile::CResourceFile()
{
	
}

#if MAC_CODE

CResource::CResource( Handle handle )
	: mHandle(handle)
{
	ResID       theID;
	ResType		theType;
	Str255		theName;
	GetResInfo( mHandle, &theID, &theType, theName );
	mID = theID;
	mName = std::string(&theName[1], theName[0]);
}

CResource::operator bool()
{
	return mHandle;
}

int16_t				CResource::GetID()
{
	return mID;
}

const std::string&	CResource::GetName()
{
	return mName;
}

const char*			CResource::GetBuffer()
{
	return *mHandle;
}

Handle				CResource::GetHandle()
{
	return mHandle;
}

size_t				CResource::GetSize()
{
	return GetHandleSize(mHandle);
}

OSType		CResourceFile::MakeOSType( const std::string& fpath )
{
	return  (OSType(fpath[0]) << 24) |
			(OSType(fpath[1]) << 16) |
			(OSType(fpath[2]) <<  8) |
			 OSType(fpath[3]);
}

void		CResourceFile::LoadFile( const std::string& fpath )
{
	FSRef		fileRef;
	mResRefNum = -1;
	
	OSStatus	resErr = FSPathMakeRef( (const UInt8*) fpath.c_str(), &fileRef, NULL );
	if( resErr == noErr )
	{
		mResRefNum = FSOpenResFile( &fileRef, fsRdPerm );
		if( mResRefNum < 0 )
		{
			fprintf( stderr, "Warning: No Mac resource fork to import.\n" );
			resErr = fnfErr;
		}
	}
	else
	{
		fprintf( stderr, "Error: Error %d locating input file's resource fork.\n", (int)resErr );
		mResRefNum = -1;
	}
}

void		CResourceFile::Close()
{
	if( mResRefNum > 0 )
	{
		CloseResFile( mResRefNum );
	}
}

int16_t	CResourceFile::Count( const std::string& type )
{
	if( mResRefNum > 0 )
	{
		return Count1Resources(MakeOSType(type));
	}
	else
	{
		return 0;
	}
}

CResource	CResourceFile::GetByID( const std::string& type, int16_t id )
{
	if( mResRefNum > 0 )
	{
		return CResource( Get1Resource( type, id ) );
	}
	else
	{
		return CResource( NULL );
	}
}

CResource	CResourceFile::GetByIndex( const std::string& type, int16_t index )
{
	if( mResRefNum > 0 )
	{
		return CResource( Get1IndResource( type, index ) );
	}
	else
	{
		return CResource( NULL );
	}
}

#else //MAC_CODE

#endif //MAC_CODE
