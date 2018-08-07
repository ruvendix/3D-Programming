//////////////////////////////////////////////////////////////////////
// 작성 날짜 : 2018.08.07
// 작성자    : Ruvendix
// 파일 내용 : 공동으로 사용되는 전역 함수 모음입니다.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_FUNCTION_H__
#define GLOBAL_FUNCTION_H__

namespace RX
{

	// printf()를 사용하는 것처럼 서식 문자열이 필요합니다.
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

	// 메시지 박스로 오류를 보여주는 함수입니다.
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

	// 에러 핸들러입니다.
	// "DxErr.h"를 이용해서 에러를 좀 더 자세히 보여줍니다.
	// HRESULT로 반환하는 함수일 때만 사용할 수 있습니다.
	// 오류가 발생하면 "legacy_stdio_definitions.lib"를 추가해주세요.
	void ErrorHandler(const CHAR* szErrName, const CHAR* szErrText)
	{
		CHAR szErr[DEFAULT_STRING_LENGTH];
		_snprintf_s(szErr, DEFAULT_STRING_LENGTH, "Error : %s\nText : %s", szErrName, szErrText);
		ShowErrorMessageBoxA(szErr, __FILE__, __LINE__);
		__debugbreak(); // 디버그 모드일 때 중단점이 됩니다.
	}
}
#endif