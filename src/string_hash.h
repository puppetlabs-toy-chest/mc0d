#ifndef STRING_HASH_H_
#define STRING_HASH_H_

// stolen from http://siliconkiwi.blogspot.co.uk/2012/04/c11-string-switch.html with some fixes

// FNV-1a constants
static constexpr unsigned long long basis = 14695981039346656037ULL;
static constexpr unsigned long long prime = 1099511628211ULL;

// compile-time hash helper function
constexpr unsigned long long hash_one(char c, const char* remain, unsigned long long value)
{
    return c == 0 ? value : hash_one(remain[0], remain + 1, (value ^ c) * prime);
}

// compile-time hash
constexpr unsigned long long hash_(const char* str)
{
    return hash_one(str[0], str + 1, basis);
}

constexpr unsigned long long operator"" _hash( const char* str, size_t n ) {
    return hash_( str );
}

#endif
