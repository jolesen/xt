#ifndef _COMMON_CORE_CORE_H_
#define _COMMON_CORE_CORE_H_

#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>

// ---------------------------------------------------------
// basic type, uint(size: 4bytes, max: 4294967295), ulong(size: 8bytes, max: 18446744073709551615)
typedef unsigned char uchar;
typedef unsigned int  uint;
typedef uint64_t      ulong;

// ---------------------------------------------------------
// _S2TYPE_
#define _S2TYPE_(T, ST)                   \
    struct T                              \
    {                                     \
        ST id, value;                     \
        T()           : id(0), value(0){} \
        T(ST i)       : id(i), value(0){} \
        T(ST i, ST v) : id(i), value(v){} \
    };

// _S3TYPE_
#define _S3TYPE_(T, ST)                                  \
    struct T                                             \
    {                                                    \
        ST type, id, value;                              \
        T()                 : type(0), id(0), value(0){} \
        T(ST t)             : type(t), id(0), value(0){} \
        T(ST t, ST i)       : type(t), id(i), value(0){} \
        T(ST t, ST i, ST v) : type(t), id(i), value(v){} \
    };

_S2TYPE_(S2UInt,  uint);
_S2TYPE_(S2ULong, ulong);
_S3TYPE_(S3UInt,  uint);
_S3TYPE_(S3ULong, ulong);

#undef _S2TYPE_
#undef _S3TYPE_

// ---------------------------------------------------------
// list
typedef std::vector<bool>         BoolList;
typedef std::vector<uchar>        UCharList;
typedef std::vector<uint>         UIntList;
typedef std::vector<ulong>        ULongList;
typedef std::vector<std::string>  StringList;
typedef std::vector<S2UInt>       S2UIntList;
typedef std::vector<S2ULong>      S2ULongList;
typedef std::vector<S3UInt>       S3UIntList;
typedef std::vector<S3ULong>      S3ULongList;

// map
typedef std::map<uint, bool>                UIntBoolMap;
typedef std::map<uint, uchar>               UIntUCharMap;
typedef std::map<uint, uint>                UIntUIntMap;
typedef std::map<uint, ulong>               UIntULongMap;
typedef std::map<uint, std::string>         UIntStringMap;
typedef std::map<uint, S2UInt>              UIntS2UIntMap;
typedef std::map<uint, S2ULong>             UIntS2ULongMap;
typedef std::map<uint, S3UInt>              UIntS3UIntMap;
typedef std::map<uint, S3ULong>             UIntS3ULongMap;
typedef std::map<std::string, bool>         StringBoolMap;
typedef std::map<std::string, uchar>        StringUCharMap;
typedef std::map<std::string, uint>         StringUIntMap;
typedef std::map<std::string, ulong>        StringULongMap;
typedef std::map<std::string, std::string>  StringStringMap;
typedef std::map<std::string, S2UInt>       StringS2UIntMap;
typedef std::map<std::string, S2ULong>      StringS2ULongMap;
typedef std::map<std::string, S3UInt>       StringS3UIntMap;
typedef std::map<std::string, S3ULong>      StringS3ULongMap;

// ---------------------------------------------------------
// temp var
#define TEMP_VAR          __FUNC1__(__LINE__)
#define __FUNC1__(line)   __FUNC2__(line)
#define __FUNC2__(line)   var_##line

// for
#define FOR(M, I)         for(uint I = 0; I < (M); ++I)
#define FORLIST(L, I)     for(uint I = 0; I < (L).size(); ++I)
#define FORMAP(M, ITER)   for(typeof((M).begin()) ITER = (M).begin(); ITER != (M).end(); ++ITER)

// map find
#define IF_FIND(M, K, ITER)    typeof((M).begin()) ITER     = (M).find(K); if(ITER     != (M).end())
#define IF_FINDx(M, K)         typeof((M).begin()) TEMP_VAR = (M).find(K); if(TEMP_VAR != (M).end())
#define IF_NFIND(M, K, ITER)   typeof((M).begin()) ITER     = (M).find(K); if(ITER     == (M).end())
#define IF_NFINDx(M, K)        typeof((M).begin()) TEMP_VAR = (M).find(K); if(TEMP_VAR == (M).end())

// EWeekDay
enum EWeekDay
{
    EDaySun = 0, EDayMon, EDayTue, EDayWed, EDayThu, EDayFri, EDaySat
};

// const
#define UINT_BITS   (sizeof(uint) * 8)
#define KB       1024
#define MB       1048576
#define SECOND   1
#define MINUTE   60
#define HOUR     3600
#define DAY      86400
#define WEEK     604800
#define MSG_MAX  262144 // 消息大小限制，256KB

// compare
#define MAX(x, y)   ((x) > (y) ? (x) : (y))
#define MIN(x, y)   ((x) < (y) ? (x) : (y))

#define IS_LEAP_YEAR(YEAR)   ((YEAR % 400 == 0) || (YEAR % 4 == 0 && YEAR % 100 != 0))

// ---------------------------------------------------------
// SBytes
struct SBytes
{
    uint size;
    char bytes[MSG_MAX];

    SBytes() : size(0) {}
};

// SHost
struct SHost
{
    std::string ip;
    uint port;

    SHost() : ip(""), port(0) {}
    SHost(const std::string &strIp, uint p) : ip(strIp), port(p) {}
};


// ---------------------------------------------------------
// CSingleton
template <class T>
class CSingleton
{
public:
    static T& Instance()
    {
        // 这种方式可能更简洁，但对于大对象可能会超过静态存储区的大小??
        // 静态存储区有大小限制吗？
        // static T t;
        // return t;

        if(!mInstance)
        {
            mInstance = new T();
        }
        return *mInstance;
    }

private:
    CSingleton();
    ~CSingleton();

    CSingleton(const CSingleton<T>&);
    CSingleton<T>& operator= (const CSingleton<T> &);

private:
    static T *mInstance;
};
template<class T>
T* CSingleton<T>::mInstance = NULL;

#endif
