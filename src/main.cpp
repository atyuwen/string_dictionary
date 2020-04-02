
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include "compressed_array.h"

#include "allocator.h"
using str_allocator = tracked_allocator<char>;
using tstring = std::basic_string<char, std::char_traits<char>, str_allocator>;
using map_allocator = tracked_allocator<std::pair<tstring, int>>;

void read_data(const std::string& file, std::vector<std::string>& data)
{
	std::ifstream ifs(file);
	std::string line;
	while (getline(ifs, line))
	{
		data.emplace_back(line);
	}
}

template<typename TMap>
void copy_data(const std::vector<std::string>& data, TMap& map)
{
	str_allocator allocator = map.get_allocator();
	int index = 0;
	for (const auto& it : data)
	{
		tstring str(it.begin(), it.end(), allocator);
		map.emplace(std::move(str), index);
		index += 1;
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
	// std::map
	{
		size_t memory_in_bytes = 0;
		map_allocator allocator(memory_in_bytes);
		std::map<tstring, int, std::less<tstring>, map_allocator> map(allocator);
		copy_data(strs, map);
		std::cout << "std::map - " << memory_in_bytes << " bytes\n";
	}

	// std::unordered_map
	{
		size_t memory_in_bytes = 0;
		map_allocator allocator(memory_in_bytes);
		std::unordered_map<tstring, int, std::hash<tstring>, std::equal_to<tstring>, map_allocator> map(allocator);
		copy_data(strs, map);
		std::cout << "std::unordered_map - " << memory_in_bytes << " bytes\n";
	}

	// compressed_array
	{
		compressed_array str_array(strs);

		// test
		for (const std::string& str : strs)
		{
			int32_t id = str_array.lookup(str);
			std::string nstr = str_array.access(id);
			if (id == -1 || str != nstr)
			{
				assert(0);
				std::cout << "compressed_array - test failed. \n";
				break;
			}
		}

		std::cout << "compressed_array - " << str_array.memory_footprint() << " bytes\n";
	}

	system("pause");
	return 0;
}