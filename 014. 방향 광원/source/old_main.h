/*====================================================================================
 *
 * Copyright (C) Ruvendix. All Rights Reserved.
 *
 * <작성 날짜>
 * 2018-11-05
 *
 * <파일 내용>
 * 이전 프로젝트에 사용된 코드를 모아놓은 곳입니다.
 *
 ====================================================================================*/
#ifndef OLD_MAIN_H__
#define OLD_MAIN_H__

#include "main.h"

 // 큐브 정점 인덱스입니다.
 // 각 정점 인덱스를 보고 소스 코드랑 맞춰보세요.

 //    3---------2
 //   /|        /|
 //  / |       / |
 // 0---------1  |
 // |  7------|--6
 // | /       | /
 // |/        |/
 // 4---------5

 // ====================================================================================
 // 매크로 정의부입니다.
#define NORMAL_VECTOR_RENDERING_VERTEX   0x0001
#define NORMAL_VECTOR_RENDERING_TRIANGLE 0x0010
#define NORMAL_VECTOR_RENDERING_PLANE    0x0100


// ====================================================================================
// 함수 선언부입니다.

// 키보드 입력을 처리해주는 함수입니다.
void InputOldKeyboard();

// 삼각형에서의 법선벡터를 구하는 함수입니다.
void CalcTriangleNormal();

// 법선벡터를 렌더링하는 함수입니다.
void RenderNormalVector();

// 큐브를 생성하는 함수입니다.
// 큐브는 점 2개만 있으면 생성할 수 있습니다.
// 내부에서 점 2개의 값을 비교한 뒤에 최솟값과 최솟값으로 분류합니다.
void CreateCube(FLOAT rPoint1, FLOAT rPoint2);

// 기준이 되는 정점 8개를 넣습니다.
void InsertBaseVertex(FLOAT rPoint1, FLOAT rPoint2);

// 큐브를 생성하기 위한 정점 정보와 인덱스 정보를 설정합니다.
void InitCubeVertexAndIndex(FLOAT rPoint1, FLOAT rPoint2);

#endif