#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <chrono>

#include "vec.hpp"

using namespace std;

void test(
		const char* test_name,
		uint32_t data_size,
		uint32_t loop_count,
		void (*func)(vec<long long>&)) {
	cout << boolalpha;
	vector vect(data_size, 0);
	vec<long long> v;
	v.m_data.resize(data_size);
	iota(v.m_data.begin(), v.m_data.end(), 10);

	chrono::nanoseconds dur(0);
	bool is_valid = true;
	for (auto j = 0u; j < loop_count; j++) {
		auto start = chrono::system_clock::now();
		func(v);
		dur += chrono::system_clock::now() - start;

		for (auto u : v.m_data)
			is_valid &= u == *v.m_data.begin();
	}

	cout << test_name;
	cout << "\n\tTotal Time:   "
		<< chrono::duration_cast<chrono::milliseconds>(dur).count()
		<< " milliseconds";
	cout << "\n\tAverage Time: "
		<< chrono::duration_cast<chrono::microseconds>(dur).count() / loop_count
		<< " microseconds";
	cout << "\n\tValid Result: " << is_valid << endl << endl;
}

int main() {
	uint32_t data_size = 1000000, loop_count = 1000;

	test("Lock Once:", data_size, loop_count,
	[](vec<long long>& v) {
		thread a([&](vec<long long>& v) {
				auto [data, lg] = v.data();
				for (auto& i : data)
					i++;
			}, ref(v));
		thread b([&](vec<long long>& v) {
				auto [data, lg] = v.data();
				for (auto& i : data)
					i = 0;
			}, ref(v));

		a.join();
		b.join();
	});

	test("Sequential Lock:", data_size, loop_count,
	[](vec<long long>& v) {
		thread a([](vec<long long>& v) {
				auto [data, lg] = v.data();
				for (auto& i : data)
					i++;
				for (auto& i : data)
					i = 0;
			}, ref(v));
		a.join();
		thread b([](long long i) { i++; }, 1);
		b.join();
	});

	test("Sequential No Lock:", data_size, loop_count,
	[](vec<long long>& v) {
		thread a([](vec<long long>& v) {
				for (auto& i : v.m_data)
					i++;
			}, ref(v));
		a.join();

		thread b([](vec<long long>& v) {
				for (auto& i : v.m_data)
					i = 0;
			}, ref(v));
		b.join();
	});

	test("No Lock:", data_size, loop_count,
	[](vec<long long>& v) {
		thread a([](vec<long long>& v) {
				for (auto& i : v.m_data)
					i++;
			}, ref(v));
		thread b([](vec<long long>& v) {
				for (auto& i : v.m_data)
					i = 0;
			}, ref(v));

		a.join();
		b.join();
	});

	test("One Thread:", data_size, loop_count,
	[](vec<long long>& v) {
		lock_guard lg(v.m_mutex);
		for (auto& i : v.m_data)
			i++;
		for (auto& i : v.m_data)
			i = 0;
	});

	test("Locking Iterator", data_size, loop_count,
	[](vec<long long>& v) {
		thread a([](vec<long long>& v) {
			for (auto& i : v)
				i++;
			}, ref(v));
		a.join();
		thread b([](vec<long long>& v) {
			for (auto& i : v)
				i = 0;
			}, ref(v));

		b.join();
	});
}
