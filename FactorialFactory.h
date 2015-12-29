#ifndef FACTORIAL_FACTORY_H

#include "objbase.h"

#include <atomic>

class FactorialFactory: public IClassFactory {
    public:
        FactorialFactory(): rc_(1) { ++obj_count_; }
        ~FactorialFactory() noexcept { --obj_count_; }

        FactorialFactory(const FactorialFactory&) = delete;
        FactorialFactory operator=(const FactorialFactory&) = delete;

        [[gnu::stdcall]]
        HRESULT QueryInterface(REFIID riid, void **obj) override;

        [[gnu::stdcall]]
        ULONG AddRef() override;

        [[gnu::stdcall]]
        ULONG Release() override;

        [[gnu::stdcall]]
        HRESULT CreateInstance(IUnknown *unknownOuter, const IID& iid,
                void **obj) override;

        [[gnu::stdcall]]
        HRESULT LockServer(BOOL lock) override;

        static long object_count() { return obj_count_; }

    private:
        std::atomic<long> rc_;

        static std::atomic<long> obj_count_;
};

#define FACTORIAL_FACTORY_H
#endif
