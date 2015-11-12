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

CResource::CResource()
{
	
}

int16_t				CResource::GetID()
{
	return mID;
}

const std::string&	CResource::GetName()
{
	return mName;
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

#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <sstream>

CResource::CResource( int16_t id, const std::string& fpath )
	: mID(id)
{
	std::string				fname = fpath.substr( fpath.rfind( '/' ) );
	std::string::size_type	_1 = fname.find( '_' ), _2 = fname.find( '_', _1);
	std::ifstream			in( fpath );
	mName = fname.substr( _2+1 );
	if( in )
	{
		std::stringstream	ss;
		ss << in.rdbuf();
		mBuffer = ss.str();
	}
}

CResource::operator bool()
{
	return GetSize();
}

const char*			CResource::GetBuffer()
{
	return mBuffer.c_str();
}

size_t				CResource::GetSize()
{
	return mBuffer.size();
}

void		CResourceFile::LoadFile( const std::string& fpath )
{
	std::string::size_type  _1, _2, slash, stak = fpath.rfind( ".stak" );
	std::string				type, name, path = fpath.substr( 0, stak ) + ".rsrc";
	DIR*					dir = opendir( path.c_str() );
	dirent*					ent;
	int16_t					id;
	if( dir )
	{
		while( (ent = readdir(dir)) )
		{
			name = ent->d_name;
			_1 = name.find( '_');
			_2 = name.find( '_', _1+1 );
			type = name.substr( 0, _1 );
			id = atoi( name.substr( _1+1, _2-_1-1 ).c_str() );
			mMap[type][id] = path + '/' + name;
		}
		closedir( dir );
	}
}

void		CResourceFile::Close()
{
	
}

int16_t	CResourceFile::Count( const std::string& type )
{
	return mMap[type].size();
}

CResource	CResourceFile::GetByID( const std::string& type, int16_t id )
{
	auto	it = mMap[type].find(id);
	if( it == mMap[type].end() )
		return CResource();
	return CResource( id, it->second );
}

CResource	CResourceFile::GetByIndex( const std::string& type, int16_t index )
{
	// FIXME: This is O(N) (so a scan is quadratic) for no reason.
	auto it = mMap[type].begin();
	while( --index )
		++it;
	return CResource( it->first, it->second );
}

#endif //MAC_CODE
