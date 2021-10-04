#pragma once
#include <cstddef>
#include <vector>
#include <mutex>
#include <sstream>

template <typename T>
struct vec {
	vec() : m_data() { }
	vec(const std::vector<T>& data) : m_data(data) { }

	auto& operator[](const size_t& index) {
		std::lock_guard lg(m_mutex);
		return m_data[index];
	}

	auto data() {
		return std::pair<
			std::vector<T>&, std::lock_guard<std::mutex>>{
					m_data, m_mutex};
	}

	void push(T value) {
		std::lock_guard lg(m_mutex);
		m_data.push_back(value);
	}

	auto to_stream() {
		std::stringstream out, str;
		auto padding = 6u;

		for (auto i = 0u; i < m_data.size(); ++i) {
			if (m_data[i] >= 0)
				str << ' ';
			if (-10 < m_data[i] < 10)
				str << ' ';

			str << m_data[i];
			if (i != m_data.size() - 1)
				str << ",";
			else
				padding--;

			for (auto j = str.str().length(); j < padding; ++j)
				out << ' ';

			if ((i + 1) % 10 == 0)
				str << "\n";

			out << str.str();
			str.str("");
		}
		out << "\n";
		return out;
	}

// should be private,
// but I don't wanna wright an entire class interface
	std::vector<T> m_data;
	mutable std::mutex m_mutex;
};
