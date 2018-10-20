/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <�ۼ� ��¥>
 * 2018-08-08
 *
 * <���� ����>
 * �������� ���Ǵ� ���� ���� �����Դϴ�.
 * ����ü, ����ü ���� �ֽ��ϴ�.
 *
 ====================================================================================*/
#ifndef GLOBAL_TYPE_H__
#define GLOBAL_TYPE_H__


 // ====================================================================================
 // �Լ� ������ �����Դϴ�.
typedef HRESULT(CALLBACK* SubFunc)();
typedef HMONITOR(CALLBACK* MonitorFromWindowFunc)(HWND, DWORD);


// ====================================================================================
// ����ü�Դϴ�.
enum class PROJECT_MODE : INT32
{
	PM_DEBUG = 0,
	PM_RELEASE,
};

enum class ROUTINE_STATE : INT32
{
	NORMAL = 0,
	FAILURE,
	EXIT,
};

enum class SUBFUNC_TYPE : INT32
{
	INIT,
	UPDATE,
	RENDER,
	RELEASE,
	LOSTDEVICE,
	RESETDEVICE,
	MAX,
};


// ====================================================================================
// ����ü�Դϴ�.

// ���� ��ƾ Ÿ�԰� �Լ��� 1:1 �����Դϴ�.
struct SubFuncInfo
{
	const static INT32 MAX_SUBFUNC = static_cast<INT32>(SUBFUNC_TYPE::MAX);
	SubFunc subFunc;

	void Reset()
	{
		subFunc = nullptr;
	}
};


#endif