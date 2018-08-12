/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-08-07
 *
 * <파일 내용>
 * 공동으로 사용되는 전역 상수 모음입니다.
 * 미리 컴파일된 헤더 파일에 작성되는 헤더 파일입니다.
 *
 ====================================================================================*/
#ifndef GLOBAL_CONSTANT_H__
#define GLOBAL_CONSTANT_H__

const INT32 DEFAULT_CLIENT_WIDTH  = 1024;
const INT32 DEFAULT_CLIENT_HEIGHT = 768;

const INT32 DEFAULT_STRING_LENGTH = 512;
const INT32 MAX_STRING_LENGTH     = 1024;
const INT32 MAX_FILE_NAME         = 256;

const INT32 D3DUSAGE_NONE = 0;

// 프레임 카운트에만 사용되고
// 실제 프레임은 시간으로 측정되기 때문에
// 가변 프레임으로 작동합니다.
//const INT32 FRAME_COUNT_THIRTY = 30;
//const INT32 FRAME_COUNT_SIXTY  = 60;

const INT32 UPDATE_FRAME_PER_SECOND = 25; // 렌더링을 제외한 게임 정보 갱신 FPS
const INT32 FRAME_SKIP_TIME         = 1000 / UPDATE_FRAME_PER_SECOND;
const INT32 MAX_FRAMESKIP           = 10;

// 문자열은 미리 컴파일된 헤더 파일에서 중복 정의로 인식하므로
// 표준 라이브러리를 따라 문자열은 매크로 상수로 정의합니다.
#define SZ_WINDOW_CLASS     _T("Default_3D_Program")
#define SZ_PROGRAM_TITLE    _T("Default_3D_Program")
#define SZ_NONE             _T("None")

#endif