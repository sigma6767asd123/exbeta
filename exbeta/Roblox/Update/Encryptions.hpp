#pragma once

#include <Windows.h>

template<typename T>
struct vmvalue1
{
private:
    T Storage;

public:
    operator const T() const
    {
        return (T)((uintptr_t)Storage - (uintptr_t)this);
    }

    void operator=(const T& Value)
    {
        Storage = (T)((uintptr_t)Value + (uintptr_t)this);
    }

    const T operator->() const
    {
        return operator const T();
    }

    T Get() const
    {
        return operator const T();
    }

    void Set(const T& Value)
    {
        operator=(Value);
    }
};

template<typename T>
struct vmvalue2
{
private:
    T Storage;

public:
    operator const T() const
    {
        return (T)((uintptr_t)this - (uintptr_t)Storage);
    }

    void operator=(const T& Value)
    {
        Storage = (T)((uintptr_t)this - (uintptr_t)Value);
    }

    const T operator->() const
    {
        return operator const T();
    }

    T Get() const
    {
        return operator const T();
    }

    void Set(const T& Value)
    {
        operator=(Value);
    }
};

template<typename T>
struct vmvalue3
{
private:
    T Storage;

public:
    operator const T() const
    {
        return (T)((uintptr_t)this ^ (uintptr_t)Storage);
    }

    void operator=(const T& Value)
    {
        Storage = (T)((uintptr_t)Value ^ (uintptr_t)this);
    }

    const T operator->() const
    {
        return operator const T();
    }

    T Get() const
    {
        return operator const T();
    }

    void Set(const T& Value)
    {
        operator=(Value);
    }
};

template<typename T>
struct vmvalue4
{
private:
    T Storage;

public:
    operator const T() const
    {
        return (T)((uintptr_t)this + (uintptr_t)Storage);
    }

    void operator=(const T& Value)
    {
        Storage = (T)((uintptr_t)Value - (uintptr_t)this);
    }

    const T operator->() const
    {
        return operator const T();
    }

    T Get() const
    {
        return operator const T();
    }

    void Set(const T& Value)
    {
        operator=(Value);
    }
};

#define CLOSURE_CONT_ENC vmvalue2
#define LSTATE_STACKSIZE_ENC vmvalue3
#define PROTO_ABSLINEINFO_ENC vmvalue1
#define PROTO_DEBUGINSN_ENC vmvalue2
#define PROTO_DEBUGNAME_ENC vmvalue2
#define PROTO_LINEINFO_ENC vmvalue2
#define PROTO_LOCVARS_ENC vmvalue4
#define PROTO_SOURCE_ENC vmvalue2
#define PROTO_TYPEINFO_ENC vmvalue4
#define PROTO_UPVALUES_ENC vmvalue3
#define PROTO_USERDATA_ENC vmvalue3
#define TSTRING_HASH_ENC vmvalue4
#define UDATA_META_ENC vmvalue4


#define proto_lineinfo         PROTO_LINEINFO_ENC
#define proto_debuginsn        PROTO_DEBUGINSN_ENC
#define proto_typeinfo         PROTO_TYPEINFO_ENC
#define proto_abslineinfo      PROTO_ABSLINEINFO_ENC
#define proto_source           PROTO_SOURCE_ENC
#define proto_locvars          PROTO_LOCVARS_ENC
#define proto_upvalues         PROTO_UPVALUES_ENC
#define proto_debugname        PROTO_DEBUGNAME_ENC
#define proto_userdata         PROTO_USERDATA_ENC
#define udata_meta             UDATA_META_ENC
#define closure_cont           CLOSURE_CONT_ENC
#define tstring_hash           TSTRING_HASH_ENC
#define lstate_stacksize       LSTATE_STACKSIZE_ENC
