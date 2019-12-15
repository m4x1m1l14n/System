#ifndef __SYSTEM_WINDOWS_HELPERS_HPP__
#define __SYSTEM_WINDOWS_HELPERS_HPP__

#include <Windows.h>

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#define IsValidHandle(hdl) (((hdl) != nullptr) && ((hdl) != INVALID_HANDLE_VALUE))

#define SafeCloseHandle(hdl) {			\
	if (IsValidHandle(hdl)) {			\
		::CloseHandle(hdl);				\
		(hdl) = nullptr;				\
	}									\
}

#endif // __SYSTEM_WINDOWS_HELPERS_HPP__
