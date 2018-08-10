//////////////////////////////////////////////////////////////////////
// �ۼ� ��¥ : 2018.08.06
// �ۼ���    : Ruvendix
// ���� ���� : �������� ���Ǵ� ��� ���� �����Դϴ�.
//////////////////////////////////////////////////////////////////////
#ifndef BASE_H__
#define BASE_H__

#include <tchar.h>
#include <intrin.h> // __debugbreak()�� ����ϱ� ���� ��� �����Դϴ�.

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ���ʿ��� ������ �������� �ʱ� ���ؼ� ����մϴ�.
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