//////////////////////////////////////////////////////////////////////
// �ۼ� ��¥ : 2018.08.07
// �ۼ���    : Ruvendix
// ���� ���� : �������� ���Ǵ� ���� �Լ� �����Դϴ�. ��ũ�ο� ���� ���� �ʴ� �������Դϴ�.
//////////////////////////////////////////////////////////////////////
#ifndef GLOBAL_FUNCTION_IMPL_H__
#define GLOBAL_FUNCTION_IMPL_H__

namespace RX
{

	//////////////////////////////////////////////////////////////////////
	// <�Լ� ������Դϴ�>
	//
	void RXDebugLogImpl(const CHAR* szText);
	void RXLogImpl(PROJECT_MODE eMode, bool bMessageBox, bool bError,
		const CHAR* szFile, INT32 line,	const CHAR* szFormat, ...);
	void ShowMessageBoxImplA(const CHAR* szText);
	void ShowMessageBoxImplW(const WCHAR* szText);
	void ShowErrorMessageBoxImplA(const CHAR* szErr, const CHAR* szFileName, INT32 line);
	void ShowErrorMessageBoxImplW(const WCHAR* szErr, const WCHAR* szFileName, INT32 line);
	void DXErrorHandlerImpl(const CHAR* szErrName, const CHAR* szErrText,
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
	void RXLogImpl(PROJECT_MODE eMode, bool bMessageBox, bool bError,
		const CHAR* szFile, INT32 line, const CHAR* szFormat, ...)
	{
		CHAR szFull[MAX_STRING_LENGTH];
		_snprintf_s(szFull, _countof(szFull), "%s(%d) <%s> : ", szFile, line, __FUNCSIG__);

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

		if (bMessageBox)
		{
			if (bError)
			{
				ShowErrorMessageBoxImplA(szText, szFile, line);
			}
			else
			{
				ShowMessageBoxImplA(szText);
			}
		}

		// �����̳� ������ �α׸� ����� ��ƾ�� ���� �˴ϴ�.
		if (strlen(g_szLog) <= 0)
		{
			// �ý��� �ð��� �̿��ؼ� ���� �ð��� ���մϴ�.
			SYSTEMTIME sysTime;
			::ZeroMemory(&sysTime, sizeof(sysTime));
			WCHAR szCurTime[DEFAULT_STRING_LENGTH];
			::GetLocalTime(&sysTime);
			_snwprintf_s(szCurTime, _countof(szCurTime),
				L"%02d-%02d-%02d-%02d-%02d-%02d",
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour,
				sysTime.wMinute, sysTime.wSecond);

			// ���� �����ڵ忡�� ��Ƽ����Ʈ�� �ٲ� �ʿ�� ������
			// �ð� ���ڿ��� �����ڵ��̹Ƿ� ��� �Լ��� �����ڵ� �������� �ٲ�� �մϴ�.
			// ������ �ð� ���ڿ��� ��Ƽ����Ʈ�� ��ȯ�ϸ� ��Ƽ����Ʈ �Լ��� �����ϸ� �˴ϴ�.
			CHAR szTemp[DEFAULT_STRING_LENGTH];
			::WideCharToMultiByte(CP_ACP, 0, szCurTime, -1, szTemp, _countof(szTemp), nullptr, nullptr);

			::CreateDirectoryA("Log", nullptr);

#if defined(DEBUG) | defined(_DEBUG)
			_snprintf_s(g_szLog, _countof(g_szLog), "Log\\Debug_Program(%s).log", szTemp);
#else
			_snprintf_s(g_szLog, _countof(g_szLog), "Log\\Program(%s).log", szTemp);
#endif
		}
		
		// ���� ��Ʈ���� �ݾƾ� ���Ͽ� ������ ��ϵǹǷ� �α׸� ����� ������
		// �� ������ �ݺ��ؾ� �ؼ� ������尡 ũ���� ���� ������带 ���̴� ����� ���� ���Դϴ�.
		FILE* pLog = nullptr;
		fopen_s(&pLog, g_szLog, "at");
		fprintf(pLog, szFull);
		fclose(pLog);
	}

	// �޽��� �ڽ��� �����ִ� �Լ��Դϴ�.
	void ShowMessageBoxImplA(const CHAR* szText)
	{
		::MessageBoxA(nullptr, szText, "Caption", MB_OK);
	}

	void ShowMessageBoxImplW(const WCHAR* szText)
	{
		::MessageBoxW(nullptr, szText, L"Caption", MB_OK);
	}

	// �޽��� �ڽ��� ������ �����ִ� �Լ��Դϴ�.
	void ShowErrorMessageBoxImplA(const CHAR* szErr, const CHAR* szFileName, INT32 line)
	{
		CHAR szText[DEFAULT_STRING_LENGTH];
		_snprintf_s(szText, _countof(szText), "%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxA(nullptr, szText, "Error", MB_ICONERROR);
	}

	void ShowErrorMessageBoxImplW(const WCHAR* szErr, const WCHAR* szFileName, INT32 line)
	{
		WCHAR szText[DEFAULT_STRING_LENGTH];
		_snwprintf_s(szText, _countof(szText), L"%s\n%s(%d)", szErr, szFileName, line);
		::MessageBoxW(nullptr, szText, L"Error", MB_ICONERROR);
	}

	// ���� �ڵ鷯�Դϴ�. ���� �������Դϴ�.
	// "DxErr.h"�� �̿��ؼ� ������ �� �� �ڼ��� �����ݴϴ�.
	// HRESULT�� ��ȯ�ϴ� �Լ��� ���� ����� �� �ֽ��ϴ�.
	// ������ �߻��ϸ� "legacy_stdio_definitions.lib"�� �߰����ּ���.
	// �޽��� �ڽ� ��� ��ɵ� �ֽ��ϴ�.
	void DXErrorHandlerImpl(const CHAR* szErrName, const CHAR* szErrText, PROJECT_MODE eMode,
		const CHAR* szFileName, INT32 line)
	{
		CHAR szErr[DEFAULT_STRING_LENGTH];
		_snprintf_s(szErr, _countof(szErr), "Error : %s\nText : %s", szErrName, szErrText);
		ShowErrorMessageBoxImplA(szErr, szFileName, line);

		// �α׷ε� ����մϴ�.
		RXLogImpl(eMode, false, false, szFileName, line, "Error(%s) Text(%s)", szErrName, szErrText);

		// ����� ����� �� �ߴ����� �˴ϴ�.
		// ����Ű� ���� ���� ���α׷��� �ڵ� ����˴ϴ�.
		__debugbreak();
	}

} // namespace RX end

#endif