//////////////////////////////////////////////////////////////////////
// 작성 날짜 : 2018.08.07
// 작성자    : Ruvendix
// 파일 내용 : 공동으로 사용되는 전역 함수 모음입니다.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_FUNCTION_H__
#define GLOBAL_FUNCTION_H__

namespace RX
{

	//////////////////////////////////////////////////////////////////////
	// <함수 선언부입니다>
	//
	void RXDebugLogImpl(const CHAR* szText);
	void RXLogImpl(PROJECT_MODE eMode, bool bMessageBox, bool bError, const CHAR* szFormat, ...);
	void ShowMessageBoxA(const CHAR* szText);
	void ShowMessageBoxW(const WCHAR* szText);
	void ShowErrorMessageBoxA(const CHAR* szErr, const CHAR* szFileName, INT32 line);
	void ShowErrorMessageBoxW(const WCHAR* szErr, const WCHAR* szFileName, INT32 line);
	void DXErrorHandler(const CHAR* szErrName, const CHAR* szErrText,
		PROJECT_MODE eMode, bool bMessageBox);
	//
	//////////////////////////////////////////////////////////////////////

	// 디버그 모드에서만 확인 가능한 로그입니다.
	// 로그는 디버그 출력창에서 확인할 수 있습니다.
	void RXDebugLogImpl(const CHAR* szText)
	{
		::OutputDebugStringA(szText);
	}

	// 원칙적으로는 디버그, 릴리즈에 관계없이 파일 또는 서버에 로그를 남겨야 합니다.
	// 서식 문자열을 지원하며 "파일(라인) <함수> : 내용" 이런 형식으로 로그가 출력됩니다.
	// 참고로 디버그 모드일 때는 디버그 출력창에도 로그를 남깁니다.
	// 출력 가능한 로그의 길이는 멀티바이트 기준으로 문자 512개입니다.
	// 앞에 기본 문자열이 추가되므로 총 출력 가능한 로그의 길이는 512 + 512 = 1024입니다.
	// 메시지 박스 출력 기능도 있는데 일반 메시지 박스와 에러 메시지 박스를 지원합니다.
	void RXLogImpl(PROJECT_MODE eMode, bool bMessageBox, bool bError, const CHAR* szFormat, ...)
	{
		CHAR szFull[MAX_STRING_LENGTH];
		_snprintf_s(szFull, _countof(szFull), "%s(%d) <%s> : ", __FILE__, __LINE__, __FUNCSIG__);

		CHAR szText[DEFAULT_STRING_LENGTH];
		va_list vaList;
		va_start(vaList, szFormat);
		_vsnprintf_s(szText, _countof(szText), szFormat, vaList);
		va_end(vaList);

		strcat_s(szFull, szText);
		strcat_s(szFull, "\n");

		if (eMode == PROJECT_MODE::PM_DEBUG)
		{
			RXDebugLogImpl(szFull);
		}

		// 파일이나 서버에 로그를 남기는 루틴이 오면 됩니다.

		if (bMessageBox)
		{
			if (bError)
			{
				ShowErrorMessageBoxA(szText, __FILE__, __LINE__);
			}
			else
			{
				ShowMessageBoxA(szText);
			}
		}
	}

	// 메시지 박스를 보여주는 함수입니다.
	void ShowMessageBoxA(const CHAR* szText)
	{
		::MessageBoxA(nullptr, szText, "Caption", MB_OK);
	}

	void ShowMessageBoxW(const WCHAR* szText)
	{
		::MessageBoxW(nullptr, szText, L"Caption", MB_OK);
	}

	// 메시지 박스로 오류를 보여주는 함수입니다.
	void ShowErrorMessageBoxA(const CHAR* szErr, const CHAR* szFileName, INT32 line)
	{
		CHAR szText[DEFAULT_STRING_LENGTH];
		_snprintf_s(szText, _countof(szText), "%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxA(nullptr, szText, "Error", MB_ICONERROR);
	}

	void ShowErrorMessageBoxW(const WCHAR* szErr, const WCHAR* szFileName, INT32 line)
	{
		WCHAR szText[DEFAULT_STRING_LENGTH];
		_snwprintf_s(szText, _countof(szText), L"%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxW(nullptr, szText, L"Error", MB_ICONERROR);
	}

	// 에러 핸들러입니다.
	// "DxErr.h"를 이용해서 에러를 좀 더 자세히 보여줍니다.
	// HRESULT로 반환하는 함수일 때만 사용할 수 있습니다.
	// 오류가 발생하면 "legacy_stdio_definitions.lib"를 추가해주세요.
	// 메시지 박스 출력 기능도 있습니다.
	void DXErrorHandler(const CHAR* szErrName, const CHAR* szErrText, PROJECT_MODE eMode)
	{
		CHAR szErr[DEFAULT_STRING_LENGTH];
		_snprintf_s(szErr, _countof(szErr), "Error : %s\nText : %s", szErrName, szErrText);
		ShowErrorMessageBoxA(szErr, __FILE__, __LINE__);

		// 로그로도 출력합니다.
		RXLogImpl(eMode, false, false, "Error(%s) Text(%s)", szErrName, szErrText);

		__debugbreak(); // 디버그 모드일 때 중단점이 됩니다.
	}
}
#endif