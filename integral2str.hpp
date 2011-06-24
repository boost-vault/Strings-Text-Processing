// Copyright Alexander Nasonov, 2006
//
#ifndef FILE_integral2str_hpp_INCLUDED_F406F3KP6
#define FILE_integral2str_hpp_INCLUDED_F406F3KP6

#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/static_assert.hpp>
#include <limits>

template<class T> // T is unsigned type
inline int hibit(T un)
{
    BOOST_STATIC_ASSERT(!std::numeric_limits<T>::is_signed);
    return un >> (std::numeric_limits<T>::digits - 1);
}

template<class T>         // T is unsigned type
inline int digits_3(T un) // un in [-999, 999]
{
    BOOST_STATIC_ASSERT(!std::numeric_limits<T>::is_signed);

    if(un < 100u)
        return 1 + hibit(9u - un);
    else
        return 3;
}

template<class T>         // T is unsigned type
inline int digits_5(T un) // un in [-99999, 99999]
{
    BOOST_STATIC_ASSERT(!std::numeric_limits<T>::is_signed);

    if(un < 100u)
        return 1 + hibit(9u - un);
    else
        return un < 1000u ? 3 : 4 + hibit(9999u - un);
}

// TODO: measure performance of nlz-based
// (number of leading zeros) implementations.
#if 0
template<class T>          // T is unsigned type
inline int digits_10(T un) // un in [9999999999, -9999999999]
{
    // TODO: read about bsrl. IIRC, its result is UB when un == 0
    int msb_pos; // position of most significant bit
    __asm__ ("bsrl %1, %0" : "=r" (msb_pos) : "r" (un));

    // The alrothim is copied from http://www.hackersdelight.org

    static unsigned int table[11] = { 0, 9, 99, 999, 9999, 99999,
            999999, 9999999, 99999999, 999999999, 0xffffffff
        };

    int x = (19 * msb_pos) >> 6;
    return 1 + x + ((table[x + 1] - un) >> 31);
}
#endif

template<class T> // T is unsigned type
inline char* unsigned2str_5(T un, char* str, int digits)
{
    BOOST_STATIC_ASSERT(!std::numeric_limits<T>::is_signed);

    unsigned int digit;

    boost::uint_least32_t const multiplier4 = 8389;

    // TODO: better parallelism
    switch(digits)
    {
        case 5:
            digit = un / 10000u; // 32bits is not enough for (un*0x1a36f)>>30
            *str++ = '0' + digit;
            un -= 10000u * digit;
        case 4:
            digit = (un * multiplier4) >> 23;
            *str++ = '0' + digit;
            un -= digit * 1000u;
        case 3:
            digit = (un * 41u) >> 12;
            *str++ = '0' + digit;
            un -= digit * 100u;
        case 2:
            digit = (un * 103u) >> 10;
            *str++ = '0' + digit;
            un -= digit * 10u;
        case 1:
            *str++ = '0' + un;
    }

    return str;
}


template<class T> // T is unsigned type
inline char* unsigned2str_10(T un, char* str)
{
    // TODO: switch(digits) { case 10: ... case 9: ... }

    BOOST_STATIC_ASSERT(!std::numeric_limits<T>::is_signed);

    T hi = un / 100000u;

    if(hi == 0)
        return unsigned2str_5(un, str, digits_5(un));
    else
    {
        str = unsigned2str_5(hi, str, digits_5(hi));
        return unsigned2str_5( un % 100000u, str, 5);
    }
}

#define INTEGRAL2STR_DEFINE(T)                          \
inline bool is_negative(T n)          { return n < 0; } \
inline bool is_negative(unsigned T n) { return false; } \
inline unsigned T correct_negative(unsigned T un)       \
{ return un; }                                          \
inline unsigned T correct_negative(T n)                 \
{ unsigned T un = n; return n < 0 ? -un : un; }

// No definitions for types shorter then int
INTEGRAL2STR_DEFINE(int)
INTEGRAL2STR_DEFINE(long)
// TODO: non-standard types

#undef INTEGRAL2STR_DEFINE


template<int Digits10> struct integral2str_switch;

template<>
struct integral2str_switch<777> // generic impl
{
    template<class T>
    inline static char* doit(T un, char* str, std::size_t size)
    {
        BOOST_STATIC_ASSERT(!std::numeric_limits<T>::is_signed);

        char buf[std::numeric_limits<T>::digits10 + 2];

        char* end = buf + sizeof(buf) / sizeof(buf[0]);
        char* p = end;

        do
        {
            --p;
            *p = '0' + un % 10;
            un /= 10;
        }
        while(un != 0);

        while(p != end)
            *str++ = *p++;

        return str;
    }
};

template<>
struct integral2str_switch<10>
{
    template<class T>
    inline static char* doit(T un, char* str, std::size_t)
    {
       return unsigned2str_10(un, str);
    }
};

template<>
struct integral2str_switch<5>
{
    template<class T>
    inline static char* doit(T un, char* str, std::size_t)
    {
       return unsigned2str_5(un, str, digits_5(un));
    }
};


// Not inline
template<class T>
void integral2str_impl(T n, char* str, std::size_t size)
{
    using namespace boost;
    using mpl::_;

    typedef mpl::int_<std::numeric_limits<T>::digits10> digits10;

    typedef typename mpl::deref<
        typename mpl::find_if< mpl::vector_c<int,5,10,777>
                             , mpl::greater<_,digits10>
                             >::type
        >::type nearest;

    typedef integral2str_switch<(nearest::value)> impl;

    if(is_negative(n))
        *str++ = '-';

    str = impl::doit(correct_negative(n), str, size);
    // Note that correct_negative also promotes n
    *str = '\0';
}

template<class T>
struct resultof_integral2str
{
    BOOST_STATIC_ASSERT(std::numeric_limits<T>::is_specialized);

    typedef boost::array< char
                        , std::numeric_limits<T>::is_signed +
                          std::numeric_limits<T>::digits10 + 2
                        > type;
};

#define DEFINE_INTEGRAL2STR(T)                          \
inline resultof_integral2str<T>::type integral2str(T n) \
{ resultof_integral2str<T>::type result;                \
  char* str = result.c_array();                         \
  integral2str_impl(n, str, result.size()); return result; }

// TODO: ??? DEFINE_INTEGRAL2STR(char)
DEFINE_INTEGRAL2STR(signed char)
DEFINE_INTEGRAL2STR(unsigned char)
DEFINE_INTEGRAL2STR(signed short int)
DEFINE_INTEGRAL2STR(unsigned short int)
DEFINE_INTEGRAL2STR(signed int)
DEFINE_INTEGRAL2STR(unsigned int)
DEFINE_INTEGRAL2STR(signed long int)
DEFINE_INTEGRAL2STR(unsigned long int)
// TODO: non-standard types

#undef DEFINE_INTEGRAL2STR

#endif // #ifndef FILE_integral2str_hpp_INCLUDED_F406F3KP6

