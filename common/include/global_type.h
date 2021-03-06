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


 // ====================================================================================
 // 함수 포인터 형식입니다.
typedef HRESULT(CALLBACK* SubFunc)();
typedef HMONITOR(CALLBACK* MonitorFromWindowFunc)(HWND, DWORD);


// ====================================================================================
// 공용체입니다.
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

// 루틴 스테이트와 비슷한데
// 이것은 함수 단위로 반환 상태를 판단할 때 사용됩니다.
// 모든 함수에 사용되는 건 특수 함수에만 사용됩니다.
enum class ROUTINE_RETURN_TYPE : INT32
{
	SUCCESS = 0,
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

enum class FPS_TYPE : INT32
{
	THIRTY,
	SIXTY,
};

enum class FONT_TYPE : INT32
{
	DEFAULT,
	FPS,
};

// ====================================================================================
// 구조체입니다.

// 서브 루틴 타입과 함수는 1:1 맞춤입니다.
struct SubFuncInfo
{
	const static INT32 MAX_SUBFUNC = static_cast<INT32>(SUBFUNC_TYPE::MAX);
	SubFunc subFunc;

	void Reset()
	{
		subFunc = nullptr;
	}
};

// 텍스처 UV 좌표 관리용입니다.
struct TextureUV
{
	FLOAT u;
	FLOAT v;
};

// 예제 출력용입니다.
struct ExampleRenderInfo
{
	INT32 idx;
	INT32 value;
	const TCHAR* szValue;
};

#endif