#pragma once
#include <cstddef>
#include <vector>
#include <mutex>
#include <sstream>

template <typename T>
struct vec {
	vec() : m_data() { }
	vec(const std::vector<T>& data) : m_data(data) { }

	struct Iterator {
		Iterator(size_t index, vec<T> *vector) :
			m_index(index),
			m_lock(vector->m_mutex, std::defer_lock),
			m_vector(vector)
		{
			if (index < vector->m_data.size()) m_lock.lock();
		}

		T& operator*() const {
			return m_vector->m_data[m_index];
		}
		bool operator==(const Iterator& other) const {
			return (m_index == other.m_index
				|| (m_index >= m_vector->m_data.size()
				&& m_index >= m_vector->m_data.size()))
				&& m_vector == other.m_vector;
		}
		bool operator!=(const Iterator& other) const {
			return !((m_index == other.m_index
				|| (m_index >= m_vector->m_data.size()
				&& m_index >= m_vector->m_data.size()))
				&& m_vector == other.m_vector);
		}
		Iterator& operator++() {
			m_index++;
			return *this;
		}

	private:
		size_t m_index;
		std::unique_lock<std::mutex> m_lock;
		vec<T>* m_vector;
	};

	const Iterator begin() {
		return Iterator(0, this);
	}
	const Iterator end() {
		return Iterator(m_data.size(), this);
	}


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

// should be private,
// but I don't wanna wright an entire class interface
	std::vector<T> m_data;
	std::mutex m_mutex;
};
