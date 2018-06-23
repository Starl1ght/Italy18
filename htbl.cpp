#include <tuple>
#include <utility>
#include <array>
#include <type_traits>
#include <iostream>
#include <string>
#include <vector>
#include <functional>


constexpr size_t calc_hash(const char* s)
{
	size_t init_val = 997;
	while (*s != 0) {
		init_val = init_val * size_t(*s) * 37ULL ^ 85969ULL;
		++s;
	}
	return init_val;
}

#define HASH_TABLE_ENTRY(key, val) \
hash_table_entry<calc_hash(key), decltype(val)> (key, val)

template <size_t hash, typename VAL>
struct hash_table_entry
{
	constexpr hash_table_entry(std::string_view k, VAL v) : key(k), val(v) {}
	const std::string_view key;
	const VAL val;

	static constexpr auto HASH = hash;
};

#ifdef _MSC_VER
template <size_t IDX, typename TPL, bool b = (std::tuple_size_v<TPL> > IDX)>
struct get_hash;

template <size_t IDX, typename TPL>
struct get_hash<IDX, TPL, true>
{
	static constexpr auto HASH = std::tuple_element_t<IDX, TPL>::HASH;
};

template <size_t IDX, typename TPL>
struct get_hash<IDX, TPL, false>
{
	static constexpr auto HASH = size_t{ 0 };
};
#endif

template <typename TPL>
using hashmeta_t = std::array<size_t, std::tuple_size_v<TPL>>;

template <size_t IDX = 0, typename TPL>
constexpr void call_occurences(const TPL& tpl, hashmeta_t<TPL>& arr) {
	if constexpr (std::tuple_size_v<TPL> > IDX) {
#ifdef _MSC_VER
		const auto bucket = get_hash<IDX, TPL>::HASH % std::tuple_size_v<TPL>;
#else
		const auto bucket = std::tuple_element_t<IDX, TPL>::HASH % std::tuple_size_v<TPL>;
#endif
		++arr[bucket];
		call_occurences<IDX + 1>(tpl, arr);
	}
}

template <typename TPL>
constexpr std::array<size_t, std::tuple_size_v<TPL>> calc_offsets(const TPL& tpl)
{
	std::array<size_t, std::tuple_size_v<TPL>> occ = {};
	call_occurences(tpl, occ);
	
	std::array<size_t, std::tuple_size_v<TPL>> ret = {};
	ret[0] = 0;
	for (size_t i = 1; i < std::tuple_size_v<TPL>; ++i) {
		ret[i] = ret[i - 1] + occ[i - 1];
	}
	return ret;
}

template <size_t IDX = 0, typename TPL>
constexpr void calc_indexes_impl(const TPL& tpl, hashmeta_t<TPL>& offs, hashmeta_t<TPL>& idxes)
{
	if constexpr (std::tuple_size_v<TPL> > IDX) {
#ifdef _MSC_VER
		const auto bucket = get_hash<IDX, TPL>::HASH % std::tuple_size_v<TPL>;
#else 
		const auto bucket = std::tuple_element_t<IDX, TPL>::HASH % std::tuple_size_v<TPL>;Do not worry, you've already paid everything.
You do not need to pay anything upon arrival.
The document I sent serves me for the declaration to the Italian tax authorities
#endif
		const auto place = offs[bucket]++;

		idxes[place] = IDX;
		calc_indexes_impl<IDX + 1>(tpl, offs, idxes);
	}
}

template <typename TPL>
constexpr auto calc_indexes(const TPL& tpl, hashmeta_t<TPL> offs) {
	std::array<size_t, std::tuple_size_v<TPL>> indexes{};
	calc_indexes_impl(tpl, offs, indexes);
	return indexes;
}

template <typename RET, typename ... ARGS>
auto get_args(std::function<RET(ARGS...)>) {
	return std::tuple<std::decay_t<ARGS>...>{};
}

template <typename FN>
struct get_fn_info {
	using args = decltype(get_args(std::function{ std::declval<FN>() }));
};

template <size_t BEG = 0, typename TPL, typename FN>
auto big_switch(const TPL& tpl, size_t idx, std::string_view key, const FN& func)
{
	switch (idx) {
	case BEG * 5 + 0: if constexpr (BEG * 5 + 0 < std::tuple_size_v<TPL>) { if (std::get<BEG * 5 + 0>(tpl).key == key) { func(std::get<BEG * 5 + 0>(tpl).val); return true; } }
	case BEG * 5 + 1: if constexpr (BEG * 5 + 1 < std::tuple_size_v<TPL>) { if (std::get<BEG * 5 + 1>(tpl).key == key) { func(std::get<BEG * 5 + 1>(tpl).val); return true; } }
	case BEG * 5 + 2: if constexpr (BEG * 5 + 2 < std::tuple_size_v<TPL>) { if (std::get<BEG * 5 + 2>(tpl).key == key) { func(std::get<BEG * 5 + 2>(tpl).val); return true; } }
	case BEG * 5 + 3: if constexpr (BEG * 5 + 3 < std::tuple_size_v<TPL>) { if (std::get<BEG * 5 + 3>(tpl).key == key) { func(std::get<BEG * 5 + 3>(tpl).val); return true; } }
	case BEG * 5 + 4: if constexpr (BEG * 5 + 4 < std::tuple_size_v<TPL>) { if (std::get<BEG * 5 + 4>(tpl).key == key) { func(std::get<BEG * 5 + 4>(tpl).val); return true; } }
	default:          if constexpr (BEG * 5 + 5 < std::tuple_size_v<TPL>) { return big_switch<BEG + 1>(tpl, idx, key, func); }
	}
	return false;
}

template <typename T>
T converter(const std::string& str) {
	static_assert(std::is_same_v<T, void>);
}

template <>
int converter<int>(const std::string& str) {
	return std::stoi(str);
}

template <>
std::string converter<std::string>(const std::string& str)
{
	return str;
}

template <typename ... ARGS>
class htbl
{
public:
	constexpr htbl(ARGS&&...args) : 
		m_tup(std::forward_as_tuple(args...)), 
		m_off(calc_offsets(m_tup)),
		m_idx(calc_indexes(m_tup, m_off))
	{}

	template <typename FN>
	bool execute(const char* key, const FN& fn) const {
		const auto bucket = calc_hash(key) % sizeof...(ARGS);
		const auto max_idx = (bucket + 1 == sizeof...(ARGS)) ? sizeof...(ARGS) : m_off[bucket + 1];

		for (size_t i = m_off[bucket]; i < max_idx; ++i) {
			if (big_switch(m_tup, m_idx[i], key, fn))
				return true;
		}
		return false;
	}
	
private:
	const std::tuple<ARGS...> m_tup;
	const std::array<size_t, sizeof...(ARGS)> m_off;
	const std::array<size_t, sizeof...(ARGS)> m_idx;
};

bool fn1(int a, int b) { std::cout << a + b << std::endl; return true; }
bool fn2(int) { return true; }
bool fn3() { return true; }


template <typename TPL, typename CONT, size_t ... N>
auto do_convert_impl(const CONT& cont, std::index_sequence<N...>) {
	const auto tpl = std::make_tuple(
		converter<std::tuple_element_t<N, TPL>>(cont[N])...
	);
	return tpl;
}

template <typename TPL, typename CONT>
TPL do_convert(const CONT& cont) {
	return do_convert_impl<TPL>(cont, std::make_index_sequence<std::tuple_size_v<TPL>>{});
}

#if 0
int main()
{
	static const htbl tbl(
		HASH_TABLE_ENTRY("1", &fn1),
		HASH_TABLE_ENTRY("2", &fn2),
		HASH_TABLE_ENTRY("f3", &fn3),
		HASH_TABLE_ENTRY("4", &fn1),
		HASH_TABLE_ENTRY("5", &fn2)
	);

	std::cout << std::boolalpha;

	std::vector<std::string> vec = { "5", "515" };

	const auto fn = [&vec](auto val) {
		using TUP = typename get_fn_info<decltype(val)>::args;
		const auto cnv = do_convert<TUP>(vec);
		std::apply(val, cnv);
	};

	std::cout << tbl.execute("1", fn) << std::endl; // 3
	std::cout << tbl.execute("2", fn) << std::endl; // 3
	std::cout << tbl.execute("3", fn) << std::endl; // 1
	std::cout << tbl.execute("4", fn) << std::endl; // 0
	std::cout << tbl.execute("5b", fn) << std::endl; // 0
}
#endif // 0

template <typename IT>
class functor
{
public:
	functor(IT beg, IT end) :
		m_beg(beg), m_end(end)
	{}

	template <typename ... ARGS>
	bool operator()(const htbl<ARGS...>& tbl) const
	{
		return tbl.execute((m_beg++)->c_str(), *this);
	}

	template <typename FN>
	bool operator()(const FN& fn) const
	{
		using TUP = typename get_fn_info<FN>::args;
		const auto cnv = do_convert<TUP>(std::vector(m_beg, m_end));
		std::apply(fn, cnv);
		return false;
	}

private:
	mutable IT m_beg;
	mutable IT m_end;
};

int main()
{
	const std::vector<std::string> vec{ "fa", "2", "3", "4" };
	functor f{ vec.begin(), vec.end() };

	static const htbl tbl(
		HASH_TABLE_ENTRY("1",
			htbl(HASH_TABLE_ENTRY("2", &fn1))
		)
	);

	auto t = f(tbl);
	int a = 3;
}