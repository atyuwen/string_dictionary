
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <assert.h>

class compressed_array
{
	static constexpr uint32_t invalid_index = std::numeric_limits<uint32_t>::max();

	struct string_entry
	{
		uint32_t    index  = invalid_index;
		uint16_t    offset = 0;
		uint8_t     prefix = 0;
		uint8_t     suffix = 0;
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
		return -1;
	}

	[[nodiscard]] std::string access(int32_t id) const noexcept
	{
		assert(0 < id && id < static_cast<int>(m_strs.size()));
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
		// front coding, use dynamic programming to find the optimal schema.
		m_strs.resize(sorted.size());

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

	bool string_entry_less(const string_entry& lhs, const string_entry& rhs) noexcept
	{

	}

private:
	std::vector<char>           m_chars;
	std::vector<string_entry>   m_strs;
};

