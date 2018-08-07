//////////////////////////////////////////////////////////////////////
// �ۼ� ��¥ : 2018.08.07
// �ۼ���    : Ruvendix
// ���� ���� : �������� ���Ǵ� ���� �Լ� �����Դϴ�.
//////////////////////////////////////////////////////////////////////
#include "global_function.h"

namespace RX
{

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

	void ShowErrorMessageBoxA(const CHAR* szErr, const CHAR* szFileName, INT32 line)
	{
		CHAR szTemp[DEFAULT_STRING_LENGTH] = NONE_STRING_A;
		_snprintf_s(szTemp, DEFAULT_STRING_LENGTH, "%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxA(nullptr, szTemp, "Error", MB_ICONERROR);
	}

	void ShowErrorMessageBoxW(const WCHAR* szErr, const WCHAR* szFileName, INT32 line)
	{
		WCHAR szTemp[DEFAULT_STRING_LENGTH] = NONE_STRING_W;
		_snwprintf_s(szTemp, DEFAULT_STRING_LENGTH, L"%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxW(nullptr, szTemp, L"Error", MB_ICONERROR);
	}

	//void ErrorHandler(const CHAR* szErrName, const CHAR* szErrText, const CHAR* szFile, INT32 line)
	//{
	//	CHAR szErr[DEFAULT_STRING_LENGTH];
	//	_snprintf_s(szErr, DEFAULT_STRING_LENGTH, "Error : %s\nText : %s\nFile : %s\nLine : %d",
	//		szErrName, szErrText, szFile, line);
	//	ShowErrorMessageBoxA(szErr, szFile, line);
	//	__debugbreak(); // ����� ����� �� �ߴ����� �˴ϴ�.
	//}

}