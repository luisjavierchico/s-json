/*
 ============================================================================
 Name		: JsonParser.h
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
								
 Description	: CJsonParser declaration based on previous work from Cesar Guirao cesarg@tid.es
 ============================================================================
 */
#ifndef __JSONPARSER_H__
#define __JSONPARSER_H__

// INCLUDE FILES
#include <e32base.h>
#include "ConsumeString.h"

// CLASS DECLARATION
class CJsonParser
	{
	
public:	
	CJsonParser();
	~CJsonParser();
	
	void 	StartDecodingL( const TDesC& aData);	
	TInt 	GetParameterCount(const TDesC& aParameter);
	TBool 	GetParameterValue(const TDesC& aParameter, TAny* aValue);
	
	void StartEncoding(RBuf* aJsonEncoded);
	void closeEncoding();
	void openObject();
	void closeObject();
	void openArray();
	void closeArray();
	void addString(const TDesC& aParameter);
	void addNumber(TInt aParameter);
	void addFixedValue(TBool aParameter);
	void addParameter(const TDesC& aParameter);
	void addNext();
	
private:
	inline TBool isObject(TChar aChar);
	inline TBool isArray(TChar aChar);
	inline TBool isNumber(TChar aChar);
	inline TBool isString(TChar aChar);
	inline TBool isFixedValue(RBuf* aString);
	
	enum TJsonValueType
		{
		EJsonString = 0,
		EJsonArray,
		EJsonObject,
		EJsonFixedValue,
		EJsonNumber
		};

	typedef struct 
		{
		RBuf* 			iParam;
		TAny* 			iVal;
		TJsonValueType	iType;
		}  jsonStruct;

	TAny* 	Decode(CConsumeString* aConsumeString);
	void 	Close(RPointerArray<jsonStruct>* aJson);
		
	RBuf*	parseString(CConsumeString* aJson);
	RBuf* 	parseNumber(CConsumeString* aJson);		
	TAny* 	parseObject(CConsumeString* aJson);
	TAny* 	parseArray(CConsumeString* aJson);
	
	TBool   GetParameterValue(const TDesC& aParameter, TAny* aValue, RPointerArray<jsonStruct>* aJson);
	TInt	GetParameterCount(const TDesC& aParameter, RPointerArray<jsonStruct>* aJson);

	void PrintDebug(RPointerArray<jsonStruct>* aJson,TInt aLevel);
	
private:
	RPointerArray<jsonStruct>* 	iJson;
	RArray<TJsonValueType> 		iLastType;
	
	RBuf*						iJsonEncoded;
	};

#endif /* __JSONPARSER_H__ */

// End of File
