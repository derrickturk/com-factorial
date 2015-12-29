#define UNICODE

#define DLLEXPORT [[gnu::stdcall, gnu::dllexport]]

#include "Factorial.h"
#include "FactorialFactory.h"

#include "objbase.h"
#include "olectl.h"
#include "winreg.h"

#include <new>
#include <string>
#include <cstddef>
#include <stdexcept>
#include <memory>

using namespace std::string_literals;

namespace {

bool create_registry_key(HKEY root, const std::wstring& subkey,
        const std::wstring& value, const void *data, DWORD type,
        std::size_t size)
{
    HKEY key;
    if (RegCreateKeyEx(root, subkey.c_str(), 0, nullptr,
                REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, nullptr, &key,
                nullptr) != ERROR_SUCCESS)
        return false;
    if (RegSetValueEx(key, value.c_str(), 0, type,
                static_cast<const BYTE*>(data), static_cast<DWORD>(size))
            != ERROR_SUCCESS) {
        RegCloseKey(key);
        return false;
    }
    return RegCloseKey(key) == ERROR_SUCCESS;
}

inline bool create_registry_key(HKEY root, const std::wstring& subkey,
        const std::wstring& value, const std::wstring& data)
{
    return create_registry_key(root, subkey, value, data.c_str(), REG_SZ,
            (data.length() + 1) * sizeof(wchar_t));
}

inline bool remove_registry_key(HKEY root, const std::wstring& subkey)
{
    return RegDeleteKey(root, subkey.c_str()) == ERROR_SUCCESS;
}

std::wstring string_from_uuid(const GUID& uuid)
{
    OLECHAR str[39];
    if (StringFromGUID2(uuid, str, 39) == 0)
        throw std::logic_error("Incorrect buffer size in string_from_uuid.");
    return std::wstring(str);
}

std::wstring module_filename(HMODULE module)
{
    size_t buf_sz = 512;
    while (true) {
        std::unique_ptr<wchar_t[]> str(new wchar_t[buf_sz]);
        if (GetModuleFileName(module, str.get(), buf_sz) < buf_sz)
            return std::wstring(str.get());
        buf_sz *= 2;
    }
}

const std::wstring dll_description(L"COM Factorial Object");
const std::wstring progid(L"CppCom.Factorial");

HMODULE dll_module = nullptr;

}

extern "C" {

DLLEXPORT BOOL DllMain(HANDLE module, DWORD reason, void *)
{
    if (reason == DLL_PROCESS_ATTACH)
        dll_module = static_cast<HMODULE>(module);
    return TRUE;
}

DLLEXPORT
HRESULT DllGetClassObject(const CLSID& clsid, const IID& iid, void **obj)
{
    if (clsid != Factorial::clsid)
        return CLASS_E_CLASSNOTAVAILABLE;

    FactorialFactory* factory = new (std::nothrow) FactorialFactory;
    if (!factory)
        return E_OUTOFMEMORY;
    return factory->QueryInterface(iid, obj);
}

DLLEXPORT HRESULT DllCanUnloadNow()
{
    if (Factorial::object_count() != 0 || FactorialFactory::object_count() != 0)
        return S_FALSE;
    return S_OK;
}

DLLEXPORT HRESULT DllRegisterServer()
{
    std::wstring clsid = string_from_uuid(Factorial::clsid);
    std::wstring subkey = L"clsid\\"s + clsid;

    // default value: description of class
    if (!create_registry_key(HKEY_CLASSES_ROOT, subkey,
                L""s /* default value */, dll_description))
        return SELFREG_E_CLASS;

    // InprocServer32: DLL file name
    if (!dll_module)
        return SELFREG_E_CLASS;
    std::wstring dll_filename = module_filename(dll_module);
    if (!create_registry_key(HKEY_CLASSES_ROOT, subkey + L"\\InprocServer32",
                L""s, dll_filename))
        return SELFREG_E_CLASS;

    // ProgId: Factorial progid
    if (!create_registry_key(HKEY_CLASSES_ROOT, subkey + L"\\ProgId",
                L""s, progid))
        return SELFREG_E_CLASS;

    // write class description to default value for progid
    if (!create_registry_key(HKEY_CLASSES_ROOT, progid,
                L""s, dll_description))
        return SELFREG_E_CLASS;

    // write CLSID to clsid key for progid
    if (!create_registry_key(HKEY_CLASSES_ROOT, progid + L"\\CLSID",
                L""s, clsid))
        return SELFREG_E_CLASS;

    return S_OK;
}

DLLEXPORT HRESULT DllUnregisterServer()
{
    std::wstring clsid = string_from_uuid(Factorial::clsid);
    std::wstring subkey = L"clsid\\"s + clsid;

    // must delete subtree-up, as deletion will fail if target key has subkeys
    bool success = true;
    success = success && remove_registry_key(HKEY_CLASSES_ROOT,
            subkey + L"\\InprocServer32");
    success = success && remove_registry_key(HKEY_CLASSES_ROOT,
            subkey + L"\\ProgId");
    success = success && remove_registry_key(HKEY_CLASSES_ROOT, subkey);
    success = success && remove_registry_key(HKEY_CLASSES_ROOT,
            progid + L"\\CLSID");
    success = success && remove_registry_key(HKEY_CLASSES_ROOT, progid);

    return success ? S_OK : SELFREG_E_CLASS;
}

}
