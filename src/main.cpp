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
		void (*func)(vec<int>&)) {
	cout << boolalpha;
	vec<int> v;
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

	chrono::milliseconds time = chrono::duration_cast<chrono::milliseconds>(dur);
	cout << test_name << time.count() << " ms\t";
	cout << "Valid Result: " << is_valid << endl;
}

int main() {
	uint32_t data_size = 1000000, loop_count = 1000;

	test("Lock Once:  ", data_size, loop_count,
	[](vec<int>& v) {
		thread a([&](vec<int>& v) {
				auto [data, lg] = v.data();
				for (auto& i : data)
					i++;
			}, ref(v));
		thread b([&](vec<int>& v) {
				auto [data, lg] = v.data();
				for (auto& i : data)
					i = 0;
			}, ref(v));

		a.join();
		b.join();
	});

	test("Lock Each:  ", data_size, loop_count,
	[](vec<int>& v) {
		thread a([](vec<int>& v) {
				for (auto& i : v.data().first)
					i++;
			}, ref(v));
		thread b([](vec<int>& v) {
				for (auto& i : v.data().first)
					i = 0;
			}, ref(v));

		a.join();
		b.join();
	});

	test("Seq Lock:   ", data_size, loop_count,
	[](vec<int>& v) {
		thread a([](vec<int>& v) {
				auto [data, _] = v.data();
				for (auto& i : data)
					i++;
			}, ref(v));
		a.join();

		thread b([](vec<int>& v) {
				auto [data, _] = v.data();
				for (auto& i : data)
					i = 0;
			}, ref(v));
		b.join();
	});

	test("Seq No Lck: ", data_size, loop_count,
	[](vec<int>& v) {
		thread a([](vec<int>& v) {
				for (auto& i : v.m_data)
					i++;
			}, ref(v));
		a.join();

		thread b([](vec<int>& v) {
				for (auto& i : v.m_data)
					i = 0;
			}, ref(v));
		b.join();
	});

	test("One Thread: ", data_size, loop_count,
	[](vec<int>& v) {
		for (auto& i : v.m_data)
			i++;
		for (auto& i : v.m_data)
			i = 0;
	});

	test("No Lock:    ", data_size, loop_count,
	[](vec<int>& v) {
		thread a([](vec<int>& v) {
				for (auto& i : v.m_data)
					i++;
			}, ref(v));
		thread b([](vec<int>& v) {
				for (auto& i : v.m_data)
					i = 0;
			}, ref(v));

		a.join();
		b.join();
	});
}
