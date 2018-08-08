//////////////////////////////////////////////////////////////////////
// �ۼ� ��¥ : 2018.08.07
// �ۼ���    : Ruvendix
// ���� ���� : �������� ���Ǵ� ���� �Լ� �����Դϴ�.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_FUNCTION_H__
#define GLOBAL_FUNCTION_H__

namespace RX
{

	//////////////////////////////////////////////////////////////////////
	// <�Լ� ������Դϴ�>
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

	// ����� ��忡���� Ȯ�� ������ �α��Դϴ�.
	// �α״� ����� ���â���� Ȯ���� �� �ֽ��ϴ�.
	void RXDebugLogImpl(const CHAR* szText)
	{
		::OutputDebugStringA(szText);
	}

	// ��Ģ�����δ� �����, ����� ������� ���� �Ǵ� ������ �α׸� ���ܾ� �մϴ�.
	// ���� ���ڿ��� �����ϸ� "����(����) <�Լ�> : ����" �̷� �������� �αװ� ��µ˴ϴ�.
	// ����� ����� ����� ���� ����� ���â���� �α׸� ����ϴ�.
	// ��� ������ �α��� ���̴� ��Ƽ����Ʈ �������� ���� 512���Դϴ�.
	// �տ� �⺻ ���ڿ��� �߰��ǹǷ� �� ��� ������ �α��� ���̴� 512 + 512 = 1024�Դϴ�.
	// �޽��� �ڽ� ��� ��ɵ� �ִµ� �Ϲ� �޽��� �ڽ��� ���� �޽��� �ڽ��� �����մϴ�.
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

		// �����̳� ������ �α׸� ����� ��ƾ�� ���� �˴ϴ�.

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

	// �޽��� �ڽ��� �����ִ� �Լ��Դϴ�.
	void ShowMessageBoxA(const CHAR* szText)
	{
		::MessageBoxA(nullptr, szText, "Caption", MB_OK);
	}

	void ShowMessageBoxW(const WCHAR* szText)
	{
		::MessageBoxW(nullptr, szText, L"Caption", MB_OK);
	}

	// �޽��� �ڽ��� ������ �����ִ� �Լ��Դϴ�.
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

	// ���� �ڵ鷯�Դϴ�.
	// "DxErr.h"�� �̿��ؼ� ������ �� �� �ڼ��� �����ݴϴ�.
	// HRESULT�� ��ȯ�ϴ� �Լ��� ���� ����� �� �ֽ��ϴ�.
	// ������ �߻��ϸ� "legacy_stdio_definitions.lib"�� �߰����ּ���.
	// �޽��� �ڽ� ��� ��ɵ� �ֽ��ϴ�.
	void DXErrorHandler(const CHAR* szErrName, const CHAR* szErrText, PROJECT_MODE eMode)
	{
		CHAR szErr[DEFAULT_STRING_LENGTH];
		_snprintf_s(szErr, _countof(szErr), "Error : %s\nText : %s", szErrName, szErrText);
		ShowErrorMessageBoxA(szErr, __FILE__, __LINE__);

		// �α׷ε� ����մϴ�.
		RXLogImpl(eMode, false, false, "Error(%s) Text(%s)", szErrName, szErrText);

		__debugbreak(); // ����� ����� �� �ߴ����� �˴ϴ�.
	}
}
#endif