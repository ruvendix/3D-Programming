//////////////////////////////////////////////////////////////////////
// <�ۼ� ��¥>
// 2018.08.06
//
// <�ۼ���>
// Ruvendix
//
// <���� ����>
// �������� ���Ǵ� �ּ����� ���븸 ���ԵǾ��ֽ��ϴ�.
// ��, �������� ���� ���� ��� ���ϵ鸸 ���ԵǾ��ֽ��ϴ�.
//////////////////////////////////////////////////////////////////////
#ifndef BASE_H__
#define BASE_H__

#include <tchar.h>
#include <intrin.h> // __debugbreak()�� ����ϱ� ���� ��� �����Դϴ�.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <clocale>

// ���ʿ��� ������ �������� �ʱ� ���ؼ� ����մϴ�.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef RXDLL_EXPORTS
#define DLL_DEFINE __declspec(dllexport)
#else
#define DLL_DEFINE __declspec(dllimport)
#endif

#include "global_type.h"
#include "global_constant.h"

#endif