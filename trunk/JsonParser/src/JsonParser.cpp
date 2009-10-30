/*
 ============================================================================
 Name		: JsonParser.cpp
 Author		: Luis J. Chico
 Version	: 1.1
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
								
 Description: CJsonParser implementation based on previous work from Cesar Guirao cesarg@tid.es
 ============================================================================
 */
// INCLUDE FILES
#include <coemain.h>
#include <f32file.h>
#include "JsonParser.h"

CJsonParser::CJsonParser() : iJson(0), iJsonEncoded(0)
	{
	}

CJsonParser::~CJsonParser()
	{
	if ( iJson )
		{
		Close( iJson );
		}
	
	if ( iJsonEncoded )
		{
		iJsonEncoded->Close();
		delete iJsonEncoded;
		}
	}

void CJsonParser::Close( RPointerArray<jsonStruct>* aJson )
	{
	if ( aJson == NULL )
		return;
	
	for (int x = 0 ; x< aJson->Count(); x++)
		{
		switch ( (*aJson)[x]->iType )
			{
			case EJsonFixedValue:
			case EJsonString:
			case EJsonNumber:
								
				(*aJson)[x]->iParam->Close();
				((RBuf*)(*aJson)[x]->iVal)->Close();
				delete (*aJson)[x]->iParam;
				delete ((RBuf*)(*aJson)[x]->iVal);				
				
				break;
			case EJsonArray:
			case EJsonObject:				

				Close((RPointerArray<jsonStruct>*)(*aJson)[x]->iVal);
				
				if ((*aJson)[x]->iParam) 
					{
					(*aJson)[x]->iParam->Close();
					delete (*aJson)[x]->iParam;
					}

				break;
			default:
				RDebug::Printf("ERROR DURING JSON WALK");
				return;
			}
		delete (*aJson)[x];
		}
	aJson->Close();
	delete (aJson);
	}
/*
 * CJsonParser::StartDecodingL()
 * 
 * Parses plain json string into an internal json structure
 */
void CJsonParser::StartDecodingL(const TDesC& aJson)
	{	
	iLastType = RArray<TJsonValueType>();
	CConsumeString* string = CConsumeString::NewL(aJson);

	iJson = (RPointerArray<jsonStruct>*)Decode( string );

	delete (string);
	iLastType.Close();
	}

TInt CJsonParser::GetParameterCount(const TDesC& aParameter, RPointerArray<jsonStruct>* aJson)
	{
	if (aParameter != KNullDesC)
		{
		TInt i;
		i = aParameter.Locate(']');
		
		// non string parameters
		TLex tmp = TLex(aParameter.Mid(1,i-1));
		TInt tmpIndex;
		if ( tmp.Val(tmpIndex) == KErrNone )
			{
			if ( tmpIndex < aJson->Count() )
				if ( (*aJson)[tmpIndex]->iType == EJsonArray || (*aJson)[tmpIndex]->iType == EJsonObject )
					return GetParameterCount(  aParameter.Right(aParameter.Length()-i-1),(RPointerArray<jsonStruct>*)(*aJson)[tmpIndex]->iVal );
			}
		else
			{
			// string parameters
			for ( TInt x = 0 ; x< aJson->Count(); x++ )
				{
				if ( (*aJson)[x]->iParam != NULL && ((*aJson)[x]->iParam)->Compare(aParameter.Mid(1,i-1)) == 0 )
					if ( (*aJson)[x]->iType == EJsonArray || (*aJson)[x]->iType == EJsonObject )
						return GetParameterCount(  aParameter.Right(aParameter.Length()-i-1),(RPointerArray<jsonStruct>*)(*aJson)[x]->iVal );					
					else
						return 0;
				}
			}
		return 0;
		}
	else
		return aJson->Count();
	}

/*
 * CJsonParser::GetParameterCount()
 * 
 * Returns number of elements in a json element
 * i.ex: GetParameterCount(_L("[test][1][2]"));
 */
TInt CJsonParser::GetParameterCount(const TDesC& aParameter)
	{
	if (iJson == NULL)
		return 0;
	else
		return GetParameterCount(aParameter,iJson);
	}

/*
 * CJsonParser::GetParameterValue()
 * 
 * Returns by reference an element's value, on success function returns TRUE
 * i.ex: GetParameterValue(_L("[test][1][2]"), &value_to_recover );
 */
TInt CJsonParser::GetParameterValue(const TDesC& aParameter, TAny* aValue)
	{
	if (iJson == NULL)
		return 0;
	else
		return GetParameterValue(aParameter,aValue,iJson);
	}

TInt CJsonParser::GetParameterValue(const TDesC& aParameter, TAny* aValue,RPointerArray<jsonStruct>* aJson)
	{
	if (aParameter != KNullDesC)
		{
		TInt i;
		i = aParameter.Locate(']');
		
		// non string parameters
		TLex tmp = TLex(aParameter.Mid(1,i-1));
		TInt tmpIndex;
		if ( tmp.Val(tmpIndex) == KErrNone )
			{
			if ( tmpIndex < aJson->Count() )
				if ( (*aJson)[tmpIndex]->iType == EJsonArray || (*aJson)[tmpIndex]->iType == EJsonObject )
					return GetParameterValue(  aParameter.Right(aParameter.Length()-i-1),aValue,(RPointerArray<jsonStruct>*)(*aJson)[tmpIndex]->iVal );
			}
		else
			{
			// string parameters
			for ( TInt x = 0 ; x< aJson->Count(); x++ )
				{
				if ( (*aJson)[x]->iParam != NULL && ((*aJson)[x]->iParam)->Compare(aParameter.Mid(1,i-1)) == 0 )
					{
					switch ( (*aJson)[x]->iType )
						{
						case EJsonArray:
						case EJsonObject:
							return GetParameterValue(  aParameter.Right(aParameter.Length()-i-1),aValue,(RPointerArray<jsonStruct>*)(*aJson)[x]->iVal );
						
						case EJsonFixedValue:
							if ( (((TDesC*)(*aJson)[x]->iVal)->Compare(_L("true"))) == 0 )
								*((int*)aValue) = ETrue;
							else
								*((int*)aValue) = EFalse;
							return ETrue;
							break;
							
						case EJsonNumber:
						case EJsonString:							
							if ( ((TDes*)aValue)->MaxLength() < ((*aJson)[x]->iParam)->Length() )
								return EFalse;
							((TDes*)aValue)->Copy( (*(RBuf*)(*aJson)[x]->iVal) );
							return ETrue;
						}
					}
				}
			}
		return EFalse;
		}
	else
		return EFalse;
	}

TAny* CJsonParser::Decode(CConsumeString* aJson)
	{
	aJson->skipSpace();
	
	if ( isArray(aJson->charAt(0)) )
		{
		iLastType.Append(EJsonArray);
		return parseArray(aJson);
		}
	else if ( isObject(aJson->charAt(0)) )
		{
		iLastType.Append(EJsonObject);
		return parseObject(aJson);
		}
	else if ( isString(aJson->charAt(0)) )
		{
		iLastType.Append(EJsonString);
		return parseString(aJson);
		}
	else if ( isNumber(aJson->charAt(0)) )
		{
		iLastType.Append(EJsonNumber);
		return parseNumber(aJson);
		}
	else
		{
		// String
		RBuf* jsonString = aJson->getToken(_L(" \t\r\n,}]"));
		if ( jsonString )
			{
			if ( isFixedValue( jsonString ) )
				{
				iLastType.Append(EJsonFixedValue);
				return jsonString;
                }
            }
		jsonString->Close();
		delete jsonString;
		
        return NULL;
		}
	}

RBuf* CJsonParser::parseString(CConsumeString* aJson)
	{
	RBuf *string = new RBuf();
	aJson->consumeFirst();
	TChar c = aJson->charAt(0);
	while ( c != '"' )
		{
		TInt errno;
		if ( ( errno = string->ReAlloc(string->Length()+1)) != KErrNone )
			{
			User::Panic(_L("CJsonParser: no memory"),errno);
			}
		
		if ( c == '\\' )
			{
			TChar escape = aJson->charAt(1);
			switch (escape)
				{
				case 'n':
					string->Append('\n');
					break;
				case 'r':
					string->Append('\r');
					break;
				case '"':
					string->Append('"');
					break;
				case '\\':
					string->Append('\\');
					break;
				case '/':
					string->Append('/');
					break;
				case 't':
					string->Append('\t');
					break;
				case 'u':
					// TODO: for-hex-digits
					break;
				}
			aJson->consumeFirst();
			}
		else
			{
			string->Append(c);
			}
		aJson->consumeFirst();
		c = aJson->charAt(0);
		}
	aJson->consumeFirst();  // skip "
	return string;
	}

TAny* CJsonParser::parseObject(CConsumeString* aJson)
	{
	RPointerArray<jsonStruct>* jsonObject = new RPointerArray<jsonStruct>();
	
	aJson->consumeFirst();	// skip {
	aJson->skipSpace();		// skip spaces
	while ( aJson->charAt(0) != '}' )
		{
		if ( aJson->charAt(0) == ',' )
			{
			aJson->consumeFirst();
			aJson->skipSpace();
			}
		
		RBuf* jsonParam;
		if ( isString(aJson->charAt(0)) )
			{			
			jsonParam = parseString(aJson);
			}
		else
			{
			jsonParam = aJson->getToken(_L(" \n\r\t:"));
			}
		aJson->skipSpace();
		aJson->consumeFirst();
		aJson->skipSpace();
		
		TAny* decodeCall = Decode(aJson);

		if ( decodeCall )
			{
			jsonStruct* jsonElement = new jsonStruct();
			jsonElement->iParam = jsonParam;
			jsonElement->iType = iLastType[iLastType.Count()-1];
			jsonElement->iVal = decodeCall;
			iLastType.Remove(iLastType.Count()-1);
			
			jsonObject->Append(jsonElement);
			}
		else
			RDebug::Printf("[WARNING][JSON] null result from decodeCall");

		aJson->skipSpace();
		}
	aJson->consumeFirst();
	
	return jsonObject;
	}

RBuf* CJsonParser::parseNumber(CConsumeString* aJson)
	{
	RBuf *string = new RBuf();
	
	TChar c = aJson->charAt(0);
	while ( c>= '0' && c<='9' )
		{
		TInt errno;
		if ( ( errno = string->ReAlloc(string->Length()+1)) != KErrNone )
			{
			User::Panic(_L("CJsonParser: no memory"),errno);
			}
		
		string->Append(c);
		aJson->consumeFirst();
		c = aJson->charAt(0);
		}
	return string;
	}

TAny* CJsonParser::parseArray(CConsumeString* aJson)
	{
	RPointerArray<jsonStruct>* jsonObject = new RPointerArray<jsonStruct>();
	
	aJson->consumeFirst();
	aJson->skipSpace();
	while ( aJson->charAt(0)!= ']' )
		{
		if ( aJson->charAt(0) == ',' )
			{
			aJson->consumeFirst();
			aJson->skipSpace();
			}
		
		TAny* decodeCall = Decode(aJson);
		
		if ( decodeCall )
			{
			jsonStruct* jsonElement = new jsonStruct();
			jsonElement->iParam = NULL;
			jsonElement->iType = iLastType[iLastType.Count()-1];
			jsonElement->iVal = decodeCall;
			iLastType.Remove(iLastType.Count()-1);
			
			jsonObject->Append((jsonStruct*)jsonElement);
			
			}
		else
			RDebug::Printf("[WARNING][JSON] null result from decodeCall");

		aJson->skipSpace();
		}
	aJson->consumeFirst();

	return jsonObject;
	}

inline TBool CJsonParser::isFixedValue(RBuf* string)
	{
	if ( !string->Compare(_L("true")) )
		return ETrue;
	else if ( !string->Compare(_L("false")) )
		return ETrue;
	else if ( !string->Compare(_L("null")) )
		return ETrue;
	else
		return EFalse;
	}

inline TBool CJsonParser::isArray(TChar aChar)
	{ 
	return (aChar == '[');
	}

inline TBool CJsonParser::isObject(TChar aChar)
	{
	return ( aChar == '{' );
	}

inline TBool CJsonParser::isString(TChar aChar)
	{
	return ( aChar == '"' );
	}

inline TBool CJsonParser::isNumber(TChar aChar)
	{
	return ( ( aChar>='0' && aChar<='9') || aChar == '+' || aChar =='-' || aChar =='.' );
	}

/*
 * CJsonParser::StartEncoding()
 * 
 * Start encoding internal structure 
 */
void CJsonParser::StartEncoding(RBuf* aJsonEncoded)
	{
	iJsonEncoded = aJsonEncoded;
	}

/*
 * CJsonParser::closeEncoding()
 * 
 * Close internal coder
 */
void CJsonParser::closeEncoding()
	{
	iJsonEncoded = NULL;
	}

/*
 * CJsonParser::openObject()
 * 
 * Open Object element 
 */
void CJsonParser::openObject()
	{
	TInt errno;
	if ( ( errno = iJsonEncoded->ReAlloc(iJsonEncoded->Length()+1)) != KErrNone )
		{
		User::Panic(_L("CJsonParser: no memory"),errno);
		}	
	
	iJsonEncoded->Append(_L("{"));
	}
/*
 * CJsonParser::closeObject()
 * 
 * Close Object element 
 */
void CJsonParser::closeObject()
	{
	TInt errno;
	
	if ( ( errno = iJsonEncoded->ReAlloc(iJsonEncoded->Length()+1)) != KErrNone )
		{
		User::Panic(_L("CJsonParser: no memory"),errno);
		}	
	
	iJsonEncoded->Append(_L("}"));	
	}
/*
 * CJsonParser::openArray()
 * 
 * Open Array element 
 */
void CJsonParser::openArray()
	{
	TInt errno;		
	if ( ( errno = iJsonEncoded->ReAlloc(iJsonEncoded->Length()+1)) != KErrNone )
		{
		User::Panic(_L("CJsonParser: no memory"),errno);
		}	
	
	iJsonEncoded->Append(_L("["));	
	}
/*
 * CJsonParser::closeArray()
 * 
 * Close Array element 
 */
void CJsonParser::closeArray()
	{
	TInt errno;		
	if ( ( errno = iJsonEncoded->ReAlloc(iJsonEncoded->Length()+1)) != KErrNone )
		{
		User::Panic(_L("CJsonParser: no memory"),errno);
		}	
	
	iJsonEncoded->Append(_L("]"));	
	}
/*
 * CJsonParser::addParameter()
 * 
 * Add parameter element
 */
void CJsonParser::addParameter(const TDesC& aParameter)
	{
	if ( aParameter.Length() != 0 )
		{
		TInt errno;		
		if ( ( errno = iJsonEncoded->ReAlloc(aParameter.Length()+iJsonEncoded->Length()+3)) != KErrNone )
			{
			User::Panic(_L("CJsonParser: no memory"),errno);
			}	
		
		iJsonEncoded->Append(_L("\""));
		iJsonEncoded->Append(aParameter);
		iJsonEncoded->Append(_L("\":"));
		}	
	}
/*
 * CJsonParser::addString()
 * 
 * Add string value element 
 */
void CJsonParser::addString(const TDesC& aParameter)
	{
	if ( aParameter.Length() != 0 )
		{
		TInt errno;		
		if ( ( errno = iJsonEncoded->ReAlloc(aParameter.Length()+iJsonEncoded->Length()+2)) != KErrNone )
			{
			User::Panic(_L("CJsonParser: no memory"),errno);
			}	
		
		iJsonEncoded->Append(_L("\""));
		iJsonEncoded->Append(aParameter);
		iJsonEncoded->Append(_L("\""));
		}	
	}
/*
 * CJsonParser::addFixedValue()
 * 
 * Add (true/false) value element 
 */
void CJsonParser::addFixedValue(TBool aParameter)
	{
	TInt errno;		
	if ( ( errno = iJsonEncoded->ReAlloc(iJsonEncoded->Length()+5)) != KErrNone )
		{
		User::Panic(_L("CJsonParser: no memory"),errno);
		}	
	
	if ( aParameter )
		iJsonEncoded->Append(_L("true"));
	else
		iJsonEncoded->Append(_L("false"));
	}
/*
 * CJsonParser::addNext()
 * 
 * Prepares next json element 
 */
void CJsonParser::addNext()
	{
	TInt errno;		
	if ( ( errno = iJsonEncoded->ReAlloc(iJsonEncoded->Length()+1)) != KErrNone )
		{
		User::Panic(_L("CJsonParser: no memory"),errno);
		}	
	
	iJsonEncoded->Append(_L(","));
	}
/*
 * CJsonParser::addNumber()
 * 
 * Add numeric element 
 */
void CJsonParser::addNumber(TInt aParameter)
	{
	TInt errno;		
	if ( ( errno = iJsonEncoded->ReAlloc(iJsonEncoded->Length()+11)) != KErrNone )
		{
		User::Panic(_L("CJsonParser: no memory"),errno);
		}	
	
	iJsonEncoded->AppendNum(aParameter);
	}

void CJsonParser::PrintDebug(RPointerArray<jsonStruct>* aJson,TInt aLevel)
	{	
	if ( aJson == NULL )
		return;
	
	for (int x = 0 ; x< aJson->Count(); x++)
		{
		switch ( (*aJson)[x]->iType )
			{
			case EJsonFixedValue:
				RDebug::Print(_L("**** level/pos: %d/%d parameter: %S value: %S  (fixed value)"),aLevel,x,
						(*aJson)[x]->iParam,
						((RBuf*)(*aJson)[x]->iVal));
				break;				
			case EJsonString:
				RDebug::Print(_L("**** level/pos: %d/%d parameter: %S value: %S  (string)"),aLevel,x,
						(*aJson)[x]->iParam,
						((RBuf*)(*aJson)[x]->iVal));
				break;
			case EJsonNumber:
				RDebug::Print(_L("**** level/pos: %d/%d parameter: %S value: %S  (number)"),aLevel,x,
						(*aJson)[x]->iParam,
						((RBuf*)(*aJson)[x]->iVal));
				break;
			case EJsonArray:
				if ((*aJson)[x]->iParam != NULL )
					RDebug::Print(_L("********* array %S"),(*aJson)[x]->iParam);
				else
					RDebug::Print(_L("********* array"));

				PrintDebug((RPointerArray<jsonStruct>*)(*aJson)[x]->iVal,aLevel+1);

				if ((*aJson)[x]->iParam != NULL )
					RDebug::Print(_L("********* end array %S"),(*aJson)[x]->iParam);
				else
					RDebug::Print(_L("********* end array"));

			break;
			case EJsonObject:
				if ((*aJson)[x]->iParam != NULL )
					RDebug::Print(_L("********* object %S"),(*aJson)[x]->iParam);
				else
					RDebug::Print(_L("********* object"));
				
				PrintDebug((RPointerArray<jsonStruct>*)(*aJson)[x]->iVal,aLevel+1);
				
				if ((*aJson)[x]->iParam != NULL )
					RDebug::Print(_L("********* object %S"),(*aJson)[x]->iParam);
				else
					RDebug::Print(_L("********* object"));
			break;
			}
		}
	}
//// End of File
