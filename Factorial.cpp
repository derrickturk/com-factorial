#include "Factorial.h"

#include <type_traits>

namespace {

template<class T> T factorial(T n) noexcept
{
    static_assert(std::is_integral<T>::value,
            "factorial<T> requires integral T");
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

}

// {C93EBE86-CEFB-4430-B7EB-F9CFECE7265E}
const GUID Factorial::clsid = { 0xc93ebe86, 0xcefb, 0x4430,
    { 0xb7, 0xeb, 0xf9, 0xcf, 0xec, 0xe7, 0x26, 0x5e } };

std::atomic<long> Factorial::obj_count_(0);

[[gnu::stdcall]]
HRESULT Factorial::QueryInterface(REFIID riid, void **obj)
{
    if (riid == IID_IUnknown) {
        *obj = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }

    if (riid == IID_IFactorial) {
        *obj = static_cast<IFactorial*>(this);
        AddRef();
        return S_OK;
    }

    *obj = nullptr;
    return E_NOINTERFACE;
}

[[gnu::stdcall]]
ULONG Factorial::AddRef()
{
    return ++rc_;
}

[[gnu::stdcall]]
ULONG Factorial::Release()
{
    long rc = --rc_;
    if (rc == 0)
        delete this;
    return rc;
}

[[gnu::stdcall]]
HRESULT Factorial::Factorial16(short n,
        /* [retval][out] */ short *result)
{
    *result = factorial(n);
    return S_OK;
}

[[gnu::stdcall]]
HRESULT Factorial::Factorial32(int n,
        /* [retval][out] */ int *result)
{
    *result = factorial(n);
    return S_OK;
}
