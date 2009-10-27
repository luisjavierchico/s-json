/*
 ============================================================================
 Name		: ConsumeString.h
 Author		: Luis J. Chico 
 Version	: 1.0
 Copyright	: S-Json is a Symbian C/C++ Json Parser easy and simple to use by 
			 LuisJavier.Chico[at]gmail[dot]com http://code.google.com/p/s-json/ (Oct-2009)

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
								
 Description 	: CConsumeString declaration based on previous work from Cesar Guirao cesarg@tid.es
 ============================================================================
 */

#ifndef __CONSUMESTRING_H__
#define __CONSUMESTRING_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  CConsumeString
 * 
 */
class CConsumeString : public CBase
	{
public:
	/**
	 * Destructor.
	 */
	~CConsumeString();

	/**
	 * Two-phased constructor.
	 */
	static CConsumeString* NewL(const TDesC &);

	/**
	 * Two-phased constructor.
	 */
	static CConsumeString* NewLC(const TDesC &);

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CConsumeString();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(const TDesC &);
	
public:
	TChar 	charAt(TInt aInc);
	void 	consumeFirst();
	void 	consume(TInt aInc);
	TInt 	left();
	void 	skipSpace();
	RBuf*	getToken(const TDesC& aInvalidChars);

private:
	TPtrC	iString;
	TInt	iIndex;
	};

#endif // __CONSUMESTRING_H__
