#include "Factorial.h"
#include "FactorialFactory.h"

#include <new>

std::atomic<long> FactorialFactory::obj_count_(0);

[[gnu::stdcall]]
HRESULT FactorialFactory::QueryInterface(REFIID riid, void **obj)
{
    if (riid == IID_IUnknown) {
        *obj = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }

    if (riid == IID_IClassFactory) {
        *obj = static_cast<IClassFactory*>(this);
        AddRef();
        return S_OK;
    }

    *obj = nullptr;
    return E_NOINTERFACE;
}

[[gnu::stdcall]]
ULONG FactorialFactory::AddRef()
{
    return ++rc_;
}

[[gnu::stdcall]]
ULONG FactorialFactory::Release()
{
    long rc = --rc_;
    if (rc == 0)
        delete this;
    return rc;
}

[[gnu::stdcall]]
HRESULT FactorialFactory::CreateInstance(IUnknown *unknownOuter, const IID& iid,
        void **obj)
{
    if (unknownOuter) // IDK
        return CLASS_E_NOAGGREGATION;

    Factorial *factorial = new (std::nothrow) Factorial;
    if (!factorial)
        return E_OUTOFMEMORY;

    return factorial->QueryInterface(iid, obj);
}

[[gnu::stdcall]]
HRESULT FactorialFactory::LockServer(BOOL lock)
{
    return E_NOTIMPL; // whatever
}
