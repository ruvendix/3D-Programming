/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-06
 *
 * <파일 내용>
 * 공동으로 사용되는 최소한의 내용만 포함되어있습니다.
 * 즉, 의존성이 별로 없는 헤더 파일들만 포함되어있습니다.
 *
 ====================================================================================*/
#ifndef BASE_H__
#define BASE_H__

#include <tchar.h>
#include <intrin.h> // __debugbreak()를 사용하기 위한 헤더 파일입니다.
#include <crtdbg.h> // 디버그 메모리 추적을 위한 헤더 파일

#include <cstdio>
#include <cstdlib>
#include <clocale>
#include <ctime>

#include <string>

#include <vector>

// SDKDDKVer.h를 포함하면 최고 수준의 가용성을 가진 Windows 플랫폼이 정의됩니다.
// 이전 Windows 플랫폼에 대해 응용 프로그램을 빌드하려는 경우에는 SDKDDKVer.h를 포함하기 전에
// WinSDKVer.h를 포함하고 _WIN32_WINNT 매크로를 지원하려는 플랫폼으로 설정하십시오.
#include <SDKDDKVer.h>

// 불필요한 내용을 포함하지 않기 위해서 사용합니다.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#ifdef RXBASEDX_EXPORTS
#define DLL_DEFINE __declspec(dllexport)
#else
#define DLL_DEFINE __declspec(dllimport)
#endif

#include "global_type.h"
#include "global_constant.h"

#endif