//////////////////////////////////////////////////////////////////////
// 작성 날짜 : 2018.08.06
// 작성자    : Ruvendix
// 파일 내용 : 공동으로 사용되는 헤더 파일 모음입니다.
//////////////////////////////////////////////////////////////////////
#ifndef BASE_H__
#define BASE_H__

#include <tchar.h>
#include <intrin.h> // __debugbreak()를 사용하기 위한 헤더 파일입니다.

#include <cstdio>
#include <cstdlib>
#include <cstring>

// 불필요한 내용을 포함하지 않기 위해서 사용합니다.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "global_type.h"
#include "global_constant.h"
#include "global_variable.h"
#include "global_function_impl.h"
#include "macro.h"

#endif