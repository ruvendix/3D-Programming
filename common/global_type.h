/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-08
 *
 * <파일 내용>
 * 공동으로 사용되는 전역 형식 모음입니다.
 * 구조체, 공용체 등이 있습니다.
 *
 ====================================================================================*/
#ifndef GLOBAL_TYPE_H__
#define GLOBAL_TYPE_H__

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

typedef HRESULT(CALLBACK* SubFunc)();

// 서브 루틴 타입과 함수는 1:1 맞춤입니다.
struct SubFuncInfo
{
	const static INT32 MAX_SUBFUNC = static_cast<INT32>(SUBFUNC_TYPE::MAX);
	SubFunc func;

	void Reset()
	{
		func = nullptr;
	}
};

#endif