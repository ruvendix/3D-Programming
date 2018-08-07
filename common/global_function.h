//////////////////////////////////////////////////////////////////////
// �ۼ� ��¥ : 2018.08.07
// �ۼ���    : Ruvendix
// ���� ���� : �������� ���Ǵ� ���� �Լ� �����Դϴ�.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_FUNCTION_H__
#define GLOBAL_FUNCTION_H__

namespace RX
{

	// printf()�� ����ϴ� ��ó�� ���� ���ڿ��� �ʿ��մϴ�.
	void OutputDebugLog(const CHAR* szFormat, ...)
	{
		CHAR szTemp[DEFAULT_STRING_LENGTH];
		va_list vaList;
		va_start(vaList, szFormat);
		_vsnprintf_s(szTemp, DEFAULT_STRING_LENGTH, szFormat, vaList);
		strcat_s(szTemp, "\n");
		va_end(vaList);
		OutputDebugStringA(szTemp);
	}

	// �޽��� �ڽ��� ������ �����ִ� �Լ��Դϴ�.
	void ShowErrorMessageBoxA(const CHAR* szErr, const CHAR* szFileName, INT32 line)
	{
		CHAR szTemp[DEFAULT_STRING_LENGTH];
		_snprintf_s(szTemp, DEFAULT_STRING_LENGTH, "%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxA(nullptr, szTemp, "Error", MB_ICONERROR);
	}

	void ShowErrorMessageBoxW(const WCHAR* szErr, const WCHAR* szFileName, INT32 line)
	{
		WCHAR szTemp[DEFAULT_STRING_LENGTH];
		_snwprintf_s(szTemp, DEFAULT_STRING_LENGTH, L"%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxW(nullptr, szTemp, L"Error", MB_ICONERROR);
	}

	// ���� �ڵ鷯�Դϴ�.
	// "DxErr.h"�� �̿��ؼ� ������ �� �� �ڼ��� �����ݴϴ�.
	// HRESULT�� ��ȯ�ϴ� �Լ��� ���� ����� �� �ֽ��ϴ�.
	// ������ �߻��ϸ� "legacy_stdio_definitions.lib"�� �߰����ּ���.
	void ErrorHandler(const CHAR* szErrName, const CHAR* szErrText)
	{
		CHAR szErr[DEFAULT_STRING_LENGTH];
		_snprintf_s(szErr, DEFAULT_STRING_LENGTH, "Error : %s\nText : %s", szErrName, szErrText);
		ShowErrorMessageBoxA(szErr, __FILE__, __LINE__);
		__debugbreak(); // ����� ����� �� �ߴ����� �˴ϴ�.
	}
}
#endif