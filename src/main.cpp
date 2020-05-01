
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <unordered_set>
#include "string_set_simple.h"
#include "string_set.h"

#include "libcsd_test.h"

#include "allocator.h"
using str_allocator = tracked_allocator<char>;
using tstring = std::basic_string<char, std::char_traits<char>, str_allocator>;
using set_allocator = tracked_allocator<tstring>;

void read_data(const std::string& file, std::vector<std::string>& data)
{
	std::ifstream ifs(file);
	std::string line;
	while (getline(ifs, line))
	{
		data.emplace_back(line);
	}
}

template<typename TSet>
void built_set(const std::vector<std::string>& data, TSet& set)
{
	str_allocator allocator = set.get_allocator();
	for (const auto& it : data)
	{
		tstring str(it.begin(), it.end(), allocator);
		set.emplace(std::move(str));
	}
}

int main()
{
	std::vector<std::string> strs;
#if _DEBUG
	read_data("data/data_small.txt", strs);
#else
	read_data("data/data.txt", strs);
#endif
	// std::set
	{
		size_t memory_in_bytes = 0;
		set_allocator allocator(memory_in_bytes);
		std::set<tstring, std::less<tstring>, set_allocator> set(allocator);
		built_set(strs, set);
		std::cout << "std::set - " << memory_in_bytes << " bytes\n";
	}

	// std::unordered_set
	{
		size_t memory_in_bytes = 0;
		set_allocator allocator(memory_in_bytes);
		std::unordered_set<tstring, std::hash<tstring>, std::equal_to<tstring>, set_allocator> set(allocator);
		built_set(strs, set);
		std::cout << "std::unordered_set- " << memory_in_bytes << " bytes\n";
	}

	// string_set_simple
	{
		string_set_simple str_set(strs);

		// test
		for (const std::string& str : strs)
		{
			int32_t id = str_set.lookup(str);
			std::string nstr = str_set.access(id);
			if (id == -1 || str != nstr)
			{
				assert(0);
				std::cout << "string_set_simple - test failed. \n";
				break;
			}
		}

		std::cout << "string_set_simple - " << str_set.memory_footprint() << " bytes\n";
	}

	// string_set_simple
	{
		string_set str_set(strs);

		// test
		for (const std::string& str : strs)
		{
			int32_t id = str_set.lookup(str);
			std::string nstr = str_set.access(id);
			if (id == -1 || str != nstr)
			{
				assert(0);
				std::cout << "string_set - test failed. \n";
				break;
			}
		}

		std::cout << "string_set - " << str_set.memory_footprint() << " bytes\n";
	}

	// lib CSD
	test_libcsd(strs);

	system("pause");
	return 0;
}