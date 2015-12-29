#ifndef FACTORIAL_H

#include "IFactorial.h"

#include <atomic>

class Factorial: public IFactorial {
    public:
        Factorial(): rc_(1) { ++obj_count_; }
        ~Factorial() noexcept { --obj_count_; }

        Factorial(const Factorial&) = delete;
        Factorial operator=(const Factorial&) = delete;

        [[gnu::stdcall]]
        HRESULT QueryInterface(REFIID riid, void **obj) override;

        [[gnu::stdcall]]
        ULONG AddRef() override;

        [[gnu::stdcall]]
        ULONG Release() override;

        [[gnu::stdcall]]
        HRESULT Factorial16(short n,
                /* [retval][out] */ short *result) override;

        [[gnu::stdcall]]
        HRESULT Factorial32(int n,
                /* [retval][out] */ int *result) override;

        static long object_count() { return obj_count_; }

        static const GUID clsid;

    private:
        std::atomic<long> rc_;

        static std::atomic<long> obj_count_;
};

#define FACTORIAL_H
#endif
