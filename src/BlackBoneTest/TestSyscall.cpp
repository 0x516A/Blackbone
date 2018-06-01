#include "Common.h"
#include <3rd_party/VersionApi.h>

namespace Microsoft::VisualStudio::CppUnitTestFramework {
    template<> inline std::wstring ToString<HWND__>( HWND__* t ) { RETURN_WIDE_STRING( t ); }
}

namespace Testing
{
    TEST_CLASS( Syscall )
    {
    public:
        TEST_METHOD( NtAllocateVirtualMemory )
        {
            constexpr SIZE_T requested = 0x3000;
            PVOID ptr = nullptr;
            SIZE_T size = requested;

            auto status = syscall::nt_syscall(
                syscall::get_index( "NtAllocateVirtualMemory" ),
                GetCurrentProcess(),
                &ptr,
                0,
                &size,
                MEM_COMMIT,
                PAGE_EXECUTE_READWRITE
            );

#ifdef USE32
            // Not ready yet
            Assert::AreEqual( STATUS_NOT_SUPPORTED, status );
#else
            AssertEx::NtSuccess( status );

            Assert::IsNotNull( ptr );
            Assert::AreEqual( requested, size );

            VirtualFree( ptr, 0, MEM_RELEASE );
#endif
        }

        TEST_METHOD( NtUserFindWindowEx )
        {
            UNICODE_STRING empty = { };
            UNICODE_STRING wndName = { };

            const wchar_t* name = L"Program Manager";
            SAFE_CALL( RtlInitUnicodeString, &wndName, name );

            auto hwnd = syscall::syscall<HWND>(
                syscall::get_index( IsWindows10OrGreater() ? L"win32u.dll" : L"user32.dll", "NtUserFindWindowEx" ),
                HWND_DESKTOP,
                nullptr,
                &empty,
                &wndName
                );

#ifdef USE32
            // Not ready yet
            Assert::AreEqual( HWND{}, hwnd );
#else
            Assert::AreNotEqual( HWND_DESKTOP, hwnd );

            wchar_t title[MAX_PATH] = { };
            GetWindowTextW( hwnd, title, _countof( title ) );

            Assert::AreEqual( title, name );
#endif
        }
    };
}