
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <assert.h>

class compressed_array
{
	static constexpr uint32_t uint32_max = std::numeric_limits<uint32_t>::max();
	static constexpr uint16_t uint16_max = std::numeric_limits<uint16_t>::max();

	struct string_entry
	{
		uint16_t    offset = 0;
		uint8_t     prefix = 0;
		uint8_t     suffix = 0;
		uint32_t    index  = uint32_max;
	};

public:
	compressed_array(const std::vector<std::string>& strs)
	{
		std::vector<std::string> sorted_strs = strs;
		std::sort(sorted_strs.begin(), sorted_strs.end());

		construct(sorted_strs);
	}

	~compressed_array() = default;

public:
	[[nodiscard]] int32_t lookup(const std::string& str) const noexcept
	{
		int32_t beg = 0;
		int32_t end = static_cast<int>(m_strs.size());
		while (beg < end)
		{
			int32_t mid = (beg + end) / 2;
			int32_t cmp = string_compare(mid, str);
			if (cmp == 0)
				return mid;
			else if (cmp < 0)
				beg = mid + 1;
			else
				end = mid;
		}
		return -1;
	}

	[[nodiscard]] std::string access(int32_t id) const noexcept
	{
		assert(0 <= id && id < static_cast<int>(m_strs.size()));
		const string_entry& entry = m_strs[id];
		if (entry.offset == 0)
		{
			return std::string{ &m_chars[0] + entry.index, entry.suffix };
		}
		else
		{
			const string_entry& base = m_strs[id - entry.offset];
			std::string str{ &m_chars[0] + base.index, entry.prefix };
			str.append(&m_chars[0] + entry.index, entry.suffix);
			return std::move(str);
		}
	}

	[[nodiscard]] size_t memory_footprint() const noexcept
	{
		return m_chars.size() + sizeof(string_entry) * m_strs.size();
	}

private:
	void construct(const std::vector<std::string>& sorted) noexcept
	{
		if (sorted.empty()) return;

		// front coding, use dynamic programming to find the optimal schema.
		int32_t count = static_cast<int32_t>(sorted.size());
		m_strs.resize(count);
		std::vector<uint32_t> cost(count * count, uint32_max);
		cost[0] = static_cast<uint32_t>(sorted[0].length());
		for (int32_t i = 1; i < count; ++i)
		{
			uint32_t ilen = static_cast<uint32_t>(sorted[i].length());
			for (int32_t j = i - 1; j >= 0; --j)
			{
				uint32_t diff = ilen - longest_common_prefix(sorted[i], sorted[j]);
				cost[i * count + j] = std::min(cost[i * count + j], cost[(i - 1) * count + j] + diff);
				cost[i * count + i] = std::min(cost[i * count + i], cost[(i - 1) * count + j] + ilen);
				if (diff == ilen) break;
			}
		}

		int32_t pos = count - 1;
		while (pos >= 0)
		{
			uint32_t optcost = uint32_max;
			int32_t offset = 0;
			for (int32_t i = 0; i <= pos; ++i)
			{
				if (cost[pos * count + pos - i] < optcost)
				{
					optcost = cost[pos * count + pos - i];
					offset = i;
				}
			}
			assert(offset <= uint16_max);
			int32_t base = pos - offset;
			for (int32_t i = base; i <= pos; ++i)
			{
				m_strs[i].offset = static_cast<uint16_t>(i - base);
				m_strs[i].prefix = (i == base) ? 0 : static_cast<uint8_t>(longest_common_prefix(sorted[i], sorted[base]));
				m_strs[i].suffix = static_cast<uint8_t>(sorted[i].length() - m_strs[i].prefix);
			}
			pos = base - 1;
		}

		m_chars.clear();
		for (size_t i = 0; i < sorted.size(); ++i)
		{
			const std::string& str = sorted[i];
			string_entry& entry = m_strs[i];
			m_strs[i].index = static_cast<uint32_t>(m_chars.size());
			m_chars.insert(m_chars.end(), &str[0] + entry.prefix, &str[0] + entry.prefix + entry.suffix);
		}

		m_chars.shrink_to_fit();
	}

	static uint16_t longest_common_prefix(const std::string& a, const std::string& b) noexcept
	{
		uint16_t alen = static_cast<uint16_t>(a.length());
		uint16_t blen = static_cast<uint16_t>(b.length());
		uint16_t lcp = 0;
		for (; lcp < alen && lcp < blen; ++lcp)
		{
			if (a[lcp] != b[lcp]) break;
		}
		return lcp;
	}

	int string_compare(int32_t id, const std::string& str) const noexcept
	{	
		const string_entry& entry = m_strs[id];
		if (entry.offset == 0)
		{
			int len = static_cast<int>(str.length());
			int result = strncmp(&m_chars[0] + entry.index, str.c_str(), entry.suffix);
			return (result != 0) ? result : static_cast<int>(entry.suffix) - len;
		}
		else
		{
			const string_entry& base = m_strs[id - entry.offset];
			int result = strncmp(&m_chars[0] + base.index, str.c_str(), entry.prefix);
			if (result == 0)
			{
				int len = static_cast<int>(str.length()) - static_cast<int>(entry.prefix);
				result = strncmp(&m_chars[0] + entry.index, str.c_str() + entry.prefix, entry.suffix);
				result = (result != 0) ? result : static_cast<int>(entry.suffix) - len;
			}
			return result;
		}
	}

private:
	std::vector<char>           m_chars;
	std::vector<string_entry>   m_strs;
};

