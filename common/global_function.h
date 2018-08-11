//////////////////////////////////////////////////////////////////////
// <작성 날짜>
// 2018.08.11
//
// <작성자>
// Ruvendix
//
// <파일 내용>
// 공동으로 사용되는 전역 함수 모음입니다.
// 함수 선언부만 있으며, 구현부는 각 프로젝트에 포함시켜야 합니다.
// 매크로에 영향을 받지 않습니다.
// DLL로 변경되었으므로 DLL 선언부와 구현부로 나눠집니다.
// 절대로 프로젝트와 전역 변수가 겹치면 안 됩니다. 겹치면 정상 작동이 보장되지 않습니다.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_FUNCTION_H__
#define GLOBAL_FUNCTION_H__

#include "base.h"

namespace RX
{

	// 유니코드 버전입니다.
	DLL_DEFINE void RXDebugLogImplW(const WCHAR* szText);
	DLL_DEFINE void RXLogImplW(PROJECT_MODE eMode, bool bMessageBox, bool bError,
		const WCHAR* szFile, INT32 line, const WCHAR* szFunSig, const WCHAR* szFormat, ...);
	DLL_DEFINE void ShowMessageBoxImplW(const WCHAR* szText);
	DLL_DEFINE void ShowErrorMessageBoxImplW(const WCHAR* szErr, const WCHAR* szFileName, INT32 line);
	DLL_DEFINE void DXErrorHandlerImplW(HRESULT DXError, PROJECT_MODE eMode, const WCHAR* szFileName,
		INT32 line, const WCHAR* szFunSig);

	// 멀티바이트 버전입니다.
	DLL_DEFINE void RXDebugLogImplA(const CHAR* szText);
	DLL_DEFINE void RXLogImplA(PROJECT_MODE eMode, bool bMessageBox, bool bError,
		const CHAR* szFile, INT32 line, const CHAR* szFunSig, const CHAR* szFormat, ...);
	DLL_DEFINE void ShowMessageBoxImplA(const CHAR* szText);
	DLL_DEFINE void ShowErrorMessageBoxImplA(const CHAR* szErr, const CHAR* szFileName, INT32 line);
	DLL_DEFINE void DXErrorHandlerImplA(HRESULT DXError, PROJECT_MODE eMode, const CHAR* szFileName,
		INT32 line, const CHAR* szFunSig);

} // namespace RX end

#endif