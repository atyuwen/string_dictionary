
#define _CRT_SECURE_NO_WARNINGS

#include "libcsd_test.h"
#include <iostream>
#include <memory>

#include <StringDictionary.h>
#include <iterators/IteratorDictStringPlain.h>

void test_libcsd(const std::vector<std::string>& strs)
{
	std::vector<std::string> sorted_strs = strs;
	std::sort(sorted_strs.begin(), sorted_strs.end());

	std::vector<char> chars;
	for (const std::string& str : sorted_strs)
	{
		chars.insert(chars.end(), str.begin(), str.end());
		chars.emplace_back('\0');
	}

	// HASHHF
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryHASHHF(it, chars.size(), 10) };
		// delete it
		std::cout << "CSD HASHHF - " << dict->getSize() << " bytes\n";
	}
	// HASHUFFDAC
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryHASHUFFDAC(it, chars.size(), 10) };
		// delete it
		std::cout << "CSD HASHUFFDAC - " << dict->getSize() << " bytes\n";
	}

	// PFC
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryPFC(it, 100) };
		// delete it
		std::cout << "CSD PFC - " << dict->getSize() << " bytes\n";
	}
	// RPFC
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryRPFC(it, 100) };
		// delete it
		std::cout << "CSD RPFC - " << dict->getSize() << " bytes\n";
	}

	// HTFC
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryHTFC(it, 100) };
		// delete it
		std::cout << "CSD HTFC - " << dict->getSize() << " bytes\n";
	}
	// HHTFC
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryHHTFC(it, 100) };
		// delete it
		std::cout << "CSD HHTFC - " << dict->getSize() << " bytes\n";
	}
	// RPHTFC
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryRPHTFC(it, 100) };
		// delete it
		std::cout << "CSD RPHTFC - " << dict->getSize() << " bytes\n";
	}


	// RPDAC
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryRPDAC(it) };
		// delete it
		std::cout << "CSD RPDAC - " << dict->getSize() << " bytes\n";
	}
	// FMINDEX
	{
		IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		std::unique_ptr<StringDictionary> dict{ new StringDictionaryFMINDEX(it, false, 4, 0) };
		// delete it
		std::cout << "CSD FMINDEX - " << dict->getSize() << " bytes\n";
	}
	// DXBW
	{
		//IteratorDictStringPlain* it = new IteratorDictStringPlain((uchar*)(&chars[0]), chars.size());
		//std::unique_ptr<StringDictionary> dict{ new StringDictionaryXBW(it) };
		//// delete it
		//std::cout << "CSD DXBW - " << dict->getSize() << " bytes\n";
	}


	
}
