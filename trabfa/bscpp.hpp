#ifndef BSCPP

#define BSCPP

#if __cplusplus < 201703L

#error "The bscpp library requires c++17. Try using -std=c++17 as argument to the compiler."

#else

#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)
#define EXAMPLE CONCAT(example, __COUNTER__)
#define EXAMPLE_IMP(EX)                                  \
    void EX();                                           \
    __attribute__((constructor(__COUNTER__ + 101))) void \
    CONCAT(register_, EX)()                              \
    {                                                    \
        details::register_example(EX);                   \
    }                                                    \
    void EX()

#define examples \
    EXAMPLE_IMP(EXAMPLE)

#define check_expect(a, b)                              \
    {                                                   \
        details::check_equal(a, b, __FILE__, __LINE__); \
    }

#define check_within(a, b, delta) details::_check_within(a, b, delta, __FILE__, __LINE__)

template <typename T>
std::string repr(const T &v);

template <typename T>
bool is_eq(const T &a, const T &b);

bool approx_eq(double a, double b, double delta)
{
    return std::fabs(a - b) <= std::fabs(delta);
}

namespace details
{
    template <typename A, typename B>
    void check_equal(const A &a, const B &b, const char *filename, int line);

    void _check_within(double a, double b, double delta, const char *filename, int line);

    using Example = std::function<void()>;
    std::vector<Example> *exs;
    std::vector<std::string> failures;
    unsigned int num_checks = 0;

    void register_example(Example example)
    {
        if (details::exs == NULL) {
            details::exs = new std::vector<Example>();
        }
        details::exs->push_back(example);
    }
} // namespace details

void run_tests()
{
    using namespace details;

    for (auto ex : *exs) {
        ex();
    }

    std::cout << "bscpp version 0.2 (2022/08/29)." << std::endl;
    std::cout << "Ran " << num_checks << " tests." << std::endl;

    if (failures.empty()) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << num_checks - failures.size() << " of the " << num_checks << " tests passed." << std::endl;
        std::cout << "Failures" << std::endl;
        for (auto failure : failures) {
            std::cout << failure << std::endl;
        }
    }
}

namespace details
{
    template <class T>
    std::string repr_iter(const T &v);

    template <class T>
    bool is_eq_iter(const T &a, const T &b);

    template <class T>
    std::string type_name();

    template <class T>
    std::string enum_name(const T &value);

    template <class T>
    constexpr size_t num_members();
} // namespace details

///////
// repr

template <class T, size_t N>
std::string repr(const T (&v)[N])
{
    if constexpr (std::is_same_v<T, char>) {
        if (v[N - 1] == '\0') {
            std::string_view s = v;
            return repr(s);
        }
    }
    return details::repr_iter(v);
}

template <class T, size_t N>
std::string repr(const std::array<T, N> &v)
{
    std::string s = "std::array<";
    s += std::string(details::type_name<T>());
    s += ", " + std::to_string(N) + "> ";
    s += details::repr_iter(v);
    return s;
}

template <class T>
std::string repr(const std::vector<T> &v)
{
    std::string s = "std::vector<";
    s += std::string(details::type_name<T>());
    s += "> ";
    s += details::repr_iter(v);
    return s;
}

template <class T>
std::string repr(const std::set<T> &v)
{
    std::string s = "std::set<";
    s += std::string(details::type_name<T>());
    s += "> ";
    s += details::repr_iter(v);
    return s;
}

template <class A, class B>
std::string repr(const std::map<A, B> &v)
{
    std::ostringstream out;
    std::string sep = "";
    out << "std::map<" << details::type_name<A>() << ", " << details::type_name<B>() << "> {";
    for (auto &[key, value] : v) {
        out << sep << "{" << repr(key) << ", " << repr(value) << "}";
        sep = ", ";
    }
    out << "}";
    return out.str();
}

template <class T>
std::string repr(const T &v)
{
    using namespace details;
    std::ostringstream out;
    out << std::boolalpha;
    // We are conservative here and only transform to std::string the content of some well kwnow types.
    if constexpr (std::is_same_v<T, char>) {
        // FIXME: escape
        out << '\'' << v << '\'';
    } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        // TODO: add a sane format for floating point number
        out << v;
    } else if constexpr (std::is_convertible_v<decltype(v), std::string_view>) {
        // FIXME: escape
        out << '"' << v << '"';
    } else if constexpr (std::is_enum_v<T>) {
        out << enum_name(v);
    } else if constexpr (std::is_standard_layout_v<T>) {
        constexpr auto num = num_members<T>();
        constexpr auto s = ", ";
        out << type_name<T>() << " {";
        if constexpr (num == 0) {
        } else if constexpr (num == 1) {
            const auto &[a] = v;
            out << repr(a);
        } else if constexpr (num == 2) {
            const auto &[a, b] = v;
            out << repr(a) << s
                << repr(b);
        } else if constexpr (num == 3) {
            const auto &[a, b, c] = v;
            out << repr(a) << s
                << repr(b) << s
                << repr(c);
        } else if constexpr (num == 4) {
            const auto &[a, b, c, d] = v;
            out << repr(a) << s
                << repr(b) << s
                << repr(c) << s
                << repr(d);
        } else if constexpr (num == 5) {
            const auto &[a, b, c, d, e] = v;
            out << repr(a) << s
                << repr(b) << s
                << repr(c) << s
                << repr(d) << s
                << repr(e);
        } else if constexpr (num == 6) {
            const auto &[a, b, c, d, e, f] = v;
            out << repr(a) << s
                << repr(b) << s
                << repr(c) << s
                << repr(d) << s
                << repr(e) << s
                << repr(f);
        } else {
            out << " too many fields ";
        }
        out << "}";
    } else {
        out << details::type_name<T>() << " {...}";
    }
    return out.str();
}

///////
// is_eq

bool is_eq(const std::string &a, const std::string &b)
{
    return a == b;
}

template <class T, size_t N>
bool is_eq(const std::array<T, N> &a, const std::array<T, N> &b)
{
    return details::is_eq_iter(a, b);
}

template <class T>
bool is_eq(const std::vector<T> &a, const std::vector<T> &b)
{
    if (a.size() != b.size()) {
        return false;
    }
    return details::is_eq_iter(a, b);
}

template <class T>
bool is_eq(const std::set<T> &a, const std::set<T> &b)
{
    if (a.size() != b.size()) {
        return false;
    }
    return details::is_eq_iter(a, b);
}

template <class A, class B>
bool is_eq(const std::map<A, B> &a, const std::map<A, B> &b)
{
    if (a.size() != b.size()) {
        return false;
    }
    for (auto &[key, value] : a) {
        if (!b.count(key) || !is_eq(value, b.at(key))) {
            return false;
        }
    }
    return true;
}

// Default implementation for is_eq
template <typename T>
bool is_eq(const T &lhs, const T &rhs)
{
    constexpr auto num = details::num_members<T>();
    if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_convertible_v<T, std::string_view> || std::is_enum_v<T>) {
        return lhs == rhs;
    } else if constexpr (num == 0) {
        return true;
    } else if constexpr (num == 1) {
        const auto &[a1] = lhs;
        const auto &[b1] = rhs;
        return is_eq(a1, b1);
    } else if constexpr (num == 2) {
        const auto &[a1, a2] = lhs;
        const auto &[b1, b2] = rhs;
        return is_eq(a1, b1) && is_eq(a2, b2);
    } else if constexpr (num == 3) {
        const auto &[a1, a2, a3] = lhs;
        const auto &[b1, b2, b3] = rhs;
        return is_eq(a1, b1) && is_eq(a2, b2) && is_eq(a3, b3);
    } else if constexpr (num == 4) {
        const auto &[a1, a2, a3, a4] = lhs;
        const auto &[b1, b2, b3, b4] = rhs;
        return is_eq(a1, b1) && is_eq(a2, b2) && is_eq(a3, b3) && is_eq(a4, b4);
    } else if constexpr (num == 5) {
        const auto &[a1, a2, a3, a4, a5] = lhs;
        const auto &[b1, b2, b3, b4, b5] = rhs;
        return is_eq(a1, b1) && is_eq(a2, b2) && is_eq(a3, b3) && is_eq(a4, b4) && is_eq(a5, b5);
    } else if constexpr (num == 6) {
        const auto &[a1, a2, a3, a4, a5, a6] = lhs;
        const auto &[b1, b2, b3, b4, b5, b6] = rhs;
        return is_eq(a1, b1) && is_eq(a2, b2) && is_eq(a3, b3) && is_eq(a4, b4) && is_eq(a5, b5) && is_eq(a6, b6);
    } else {
        static_assert(num > 6, "Too many fields!");
    }
}

// Implementation details
namespace details
{

    // From https://stackoverflow.com/a/35207812/5189607
    template <class T, class EqualTo>
    struct has_is_eq_impl {
        template <class U, class V>
        static auto test(U *) -> decltype(is_eq(std::declval<U>(), std::declval<V>()));
        template <typename, typename>
        static auto test(...) -> std::false_type;

        using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
    };

    template <class T, class EqualTo = T>
    struct has_is_eq : has_is_eq_impl<T, EqualTo>::type {
    };

    template <typename A, typename B>
    void check_equal(const A &a, const B &b, const char *filename, int line)
    {
        details::num_checks += 1;
        if constexpr (details::has_is_eq<A, B>::value) {
            if (!is_eq(a, b)) {
                std::stringstream ss;
                ss << "  " << filename << " at line " << line << ":" << std::endl
                   << "    left : " << repr(a) << std::endl
                   << "    right: " << repr(b) << std::endl;
                details::failures.push_back(ss.str());
            }
        } else {
            std::stringstream ss;
            ss << "  " << filename << " at line " << line << ":" << std::endl
               << "    cannot compare values of types "
               << details::type_name<A>()
               << " and "
               << details::type_name<B>()
               << "." << std::endl;
            details::failures.push_back(ss.str());
        }
    }

    void _check_within(double a, double b, double delta, const char *filename, int line)
    {
        details::num_checks += 1;
        if (!approx_eq(a, b, delta)) {
            std::stringstream ss;
            ss << "  " << filename << " at line " << line << ":" << std::endl
               << "    the absolute difference in not within " << delta << std::endl
               << "    left : " << repr(a) << std::endl
               << "    right: " << repr(b) << std::endl;
            details::failures.push_back(ss.str());
        }
    }

    template <class T>
    std::string repr_iter(const T &v)
    {
        std::ostringstream out;
        std::string sep = "";
        out << "{";
        for (auto &item : v) {
            out << sep << repr(item);
            sep = ", ";
        }
        out << "}";
        return out.str();
    }

    template <class T>
    bool is_eq_iter(const T &a, const T &b)
    {
        auto aa = a.begin();
        auto bb = b.begin();
        while (aa != a.end() && bb != b.end()) {
            if (!is_eq(*aa, *bb)) {
                return false;
            }
            aa++;
            bb++;
        }
        return aa == a.end() && bb == b.end();
    }

    // From https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
    template <typename T>
    constexpr std::string_view type_name_()
    {
#if defined(__clang__)
        constexpr auto prefix = std::string_view{"[T = "};
        constexpr auto suffix = "]";
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
        constexpr auto prefix = std::string_view{"with T = "};
        constexpr auto suffix = "; ";
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__MSC_VER)
        constexpr auto prefix = std::string_view{"get_type_name<"};
        constexpr auto suffix = ">(void)";
        constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error "Unsupported compiler"
#endif

        const auto start = function.find(prefix) + prefix.size();
        const auto end = function.find(suffix);
        const auto size = end - start;

        return function.substr(start, size);
    }

    const std::string string_type_name = std::string(type_name_<std::string>());

    template <typename T>
    std::string type_name()
    {
        if (std::is_same_v<std::string, T>) {
            return "std::string";
        }

        auto name = std::string(type_name_<T>());

        // normalize string_type_name to std::string
        while (name.find(string_type_name) != std::string::npos) {
            size_t i = name.find(string_type_name);
            name = name.substr(0, i) + "std::string" + name.substr(i + string_type_name.length());
        }

        // remove spaces
        auto start = std::remove(name.begin(), name.end(), ' ');
        name.erase(start, name.end());
        return name;
    }

    // From https://github.com/Neargye/magic_enum
    template <typename T, T E>
    std::string_view enum_name()
    {
#if defined(__clang__)
        constexpr auto prefix = std::string_view{", E = "};
        constexpr auto suffix = "]";
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
        constexpr auto prefix = std::string_view{"T E = "};
        constexpr auto suffix = "; ";
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#else
#error "Unsupported compiler"
#endif

        const auto start = function.find(prefix) + prefix.size();
        const auto end = function.rfind(suffix);
        const auto size = end - start;

        return function.substr(start, size);
    }

    template <typename T>
    std::string enum_name(const T &value)
    {
#define C(n)                                                              \
    if (static_cast<T>(n) == value) {                                     \
        std::string s = std::string(type_name<T>());                      \
        return s + "::" + std::string(enum_name<T, static_cast<T>(n)>()); \
    }
        C(0)
        C(1)
        C(2)
        C(3)
        C(4)
        C(5)
        C(6)
        C(7)
        C(8)
        C(9)
        C(10)
        return "?";
    }

    // From https://akrzemi1.wordpress.com/2020/10/01/reflection-for-aggregates/
    struct i {
        template <typename T>
        operator T();
    };

    template <unsigned I>
    struct tag : tag<I - 1> {
    };

    template <>
    struct tag<0> {
    };

    template <typename T>
    constexpr auto size_(tag<6>) -> decltype(T{i{}, i{}, i{}, i{}, i{}, i{}}, 0u)
    {
        return 6;
    }

    template <typename T>
    constexpr auto size_(tag<5>) -> decltype(T{i{}, i{}, i{}, i{}, i{}}, 0u)
    {
        return 5;
    }

    template <typename T>
    constexpr auto size_(tag<4>) -> decltype(T{i{}, i{}, i{}, i{}}, 0u)
    {
        return 4;
    }

    template <typename T>
    constexpr auto size_(tag<3>) -> decltype(T{i{}, i{}, i{}}, 0u)
    {
        return 3;
    }

    template <typename T>
    constexpr auto size_(tag<2>) -> decltype(T{i{}, i{}}, 0u)
    {
        return 2;
    }

    template <typename T>
    constexpr auto size_(tag<1>) -> decltype(T{i{}}, 0u)
    {
        return 1;
    }

    template <typename T>
    constexpr auto size_(tag<0>) -> decltype(T{}, 0u)
    {
        return 0;
    }

    template <class T>
    constexpr size_t num_members()
    {
        return size_<T>(tag<6>{});
    }
} // namespace details

#endif // C++ version

#endif // ifndef BSCPP
