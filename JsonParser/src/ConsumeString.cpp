/*
 ============================================================================
 Name					: ConsumeString.cpp
 Author				: Luis J. Chico
 Version			: 1.0
 Copyright		: S-Json is a Symbian C/C++ Json Parser easy and simple to use by 
 								LuisJavier.Chico[at]gmail[dot]com http://code.google.com/p/s-json/ (Aug-2009)
 								
 								## This program is free software; you can redistribute it and/or
								## modify it under the terms of the GNU General Public License as
								## published by the Free Software Foundation; either version 3 of the
								## License, or any later version.
								##
								## This program is distributed in the hope that it will be useful,
								## but WITHOUT ANY WARRANTY; without even the implied warranty of
								## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
								## General Public License for more details.
								##
								## You find a copy of the GNU General Public License in the file
								## license.txt along with this program; if not, write to the Free
								## Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
								
 Description	: CConsumeString implementation based on previous work from Cesar Guirao cesarg@tid.es
 ============================================================================
 */

#include "ConsumeString.h"

CConsumeString::CConsumeString(): iIndex(0)
	{
	// No implementation required
	}

CConsumeString::~CConsumeString()
	{
	}

CConsumeString* CConsumeString::NewLC(const TDesC8& aString)
	{
	CConsumeString* self = new (ELeave) CConsumeString();
	CleanupStack::PushL(self);
	self->ConstructL(aString);
	return self;
	}

CConsumeString* CConsumeString::NewL(const TDesC8& aString)
	{
	CConsumeString* self = CConsumeString::NewLC(aString);
	CleanupStack::Pop(); // self;
	return self;
	}

void CConsumeString::ConstructL(const TDesC8& aString)
	{
	iString.Set(aString);
	}

TChar CConsumeString::charAt(TInt aInc)
	{
	return iString[iIndex+aInc];
	}

void CConsumeString::consumeFirst()
	{
	iIndex++;
	}

void CConsumeString::consume(TInt aInc)
	{
	iIndex+=aInc;
	}

TInt CConsumeString::left()
	{
	return iString.Length()-iIndex;
	}

void CConsumeString::skipSpace()
	{
	if ( iIndex >= iString.Length() )
		return;
	
	TChar c = iString[iIndex];	
    while(c == ' ' || c == '\r' || c == '\n' || c=='\t')
    	{
        iIndex++;
        if(iIndex >= iString.Length() )
            return;
        c = iString[iIndex];
    	}
	}

RBuf8* CConsumeString::getToken(const TDesC8& aInvalidChars)
	{
	if ( iIndex >= iString.Length() )
		{
		return NULL;
		}
	
	RBuf8* string = new RBuf8();
	
	while ( aInvalidChars.Locate(iString[iIndex]) == KErrNotFound )
		{		
		TInt errno;
		if ( ( errno = string->ReAlloc(string->Length()+1)) != KErrNone )
			{
			User::Panic(_L("CConsumeString: no memory"),errno);
			}	
		
		string->Append(iString[iIndex++]);
		if ( iIndex >= iString.Length() )
			break;
		}
	
	if ( string->Length() == 0)
		{
		string->Close();
		delete string;
		return NULL;
		}

	return string;
	}
