
        S-json -- A Symbian C/C++ JSON Parser

===========================================================	

S-Json is a Symbian C/C++ Json Parser easy and simple to use.
Its code is released under the GNU GPLv3 (see COPYING FILE)

Release Version 1.0 includes:
	
	./COPYING			-- GNU GPLv3 License
	./readme.txt	-- this file
	
	./JsonParser/inc/ConsumeString.h
	./JsonParser/inc/JsonParser.h
	./JsonParser/src/ConsumeString.cpp
	./JsonParser/src/JsonParser.cpp

===========================================================	

Please refer to 
                   http://code.google.com/p/s-json/

for installation and usage instructions, troubleshooting and
everything else!

Or get in contact at LuisJavier.Chico [at] gmail [dot] com

Below are some coding and decoding examples for Symbian C++

// json decoding example
void decode_string_example()
	{
	CJsonParser de = CJsonParser();	
	de.StartDecodingL(_L8("[{\"Id\":197,\"interactions\":[{\"Id\":606,\"ticket\":\"1cb\"},{\"Id\":607,\"ticket\":\"0a782069196ec\"}],\"widgetAdDownloadUrl\":\"aaaa\"},{\"Id\":11197,\"interactions\":[{\"Id\":63406,\"ticket\":\"1cb\"},{\"Id\":602227,\"ticket\":\"0a782069196ec\"}],\"widgetAdDownloadUrl\":\"bbb\"}]"));
	TInt a = de.GetParameterCount(_L8("[0]"));
	TInt b = de.GetParameterCount(_L8("[1][interactions][1]"));

	
	CJsonParser ed = CJsonParser();
	ed.StartDecodingL(_L8("{\"DOB\":\"\\/Date(928142400000+0200)\\/\",\"Id\":9223372036854775807,\"country\":\"String content\",\"created\":\"\\/Date(928142400000+0200)\\/\",\"gender\":\"String content\",\"lastAdUpdate\":\"\\/Date(928142400000+0200)\\/\",\"province\":\"String content\",\"tags\":[{\"Id\":2147483647,\"active\":true,\"name\":\"String content\",\"rootId\":2147483647}],\"userPreferences\":{\"active\":true,\"adtiming\":\"String content\",\"adtypes\":[{\"Id\":2147483647,\"active\":true,\"name\":\"String content\",\"rootId\":2147483647}]},\"username\":\"String content\"}"));

	TBuf8<320> Id;
	int id = de.GetParameterValue(_L8("[1][interactions][1][ticket]"),&Id);
	}

// json encoding example
void encode_data_example()
	{
	CJsonParser de = CJsonParser();

	RBuf8* jsonPost = new RBuf8();
	de.StartEncoding(jsonPost);
		de.openObject();
			de.addParameter(_L8("adPointsRedeemUrl"));de.addString(_L8("String content"));de.addNext();
			de.addParameter(_L8("latestAppVersion"));
			de.openObject();
				de.addParameter(_L8("handset"));	de.addString(_L8("String content"));de.addNext();
				de.addParameter(_L8("version"));	de.addString(_L8("String content"));de.addNext();
				de.addParameter(_L8("downloadUrl"));de.addString(_L8("String content"));de.addNext();
				de.addParameter(_L8("required"));	de.addFixedValue(ETrue);
			de.closeObject();de.addNext();
			de.addParameter(_L8("recommendFriendUrl"));de.addString(_L8("String content"));
		de.closeObject();
	de.closeEncoding();	

	jsonPost->Close();
	delete (jsonPost);
	}