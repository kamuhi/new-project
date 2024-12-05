/*
	@file	UtilityMath.h
	@brief	便利補助関数の集まり
*/
#pragma once
#include <SimpleMath.h>
#include <VertexTypes.h>

namespace mylib
{
	//-----------------------------------------------------
	// ビューポート変換行列を取得する
	//-----------------------------------------------------
	inline DirectX::SimpleMath::Matrix GetViewportMatrix(
		int screenWidth,
		int screenHeight
	)
	{
		// ビューポート変換行列を計算する
		DirectX::SimpleMath::Matrix viewport = DirectX::SimpleMath::Matrix::Identity;
		viewport._11 =  screenWidth  / 2.0f;
		viewport._22 = -screenHeight / 2.0f;
		viewport._41 =  screenWidth  / 2.0f;
		viewport._42 =  screenHeight / 2.0f;

		return viewport;
	}

	//-----------------------------------------------------
	// 変換行列を取得する：スクリーン座標->ワールド座標
	//-----------------------------------------------------
	inline DirectX::SimpleMath::Matrix GetMatrix_ScreenToWorld(
		int screenWidth,
		int screenHeight,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection
	)
	{
		// ビューポート変換行列を取得する
		DirectX::SimpleMath::Matrix viewport = mylib::GetViewportMatrix(screenWidth, screenHeight);
		
		// 「ビュー→射影→ビューポート」の逆変換行列を計算する
		DirectX::SimpleMath::Matrix matrix = view * projection * viewport;
		matrix = matrix.Invert();

		return matrix;
	}

	//-----------------------------------------------------
	// マウスレイを取得する
	//-----------------------------------------------------
	inline DirectX::SimpleMath::Ray GetMouseRay(
		int mouseX,
		int mouseY,
		int screenWidth,
		int screenHeight,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection
	)
	{
		// 「ビュー→射影→ビューポート」の逆変換行列を計算する
		DirectX::SimpleMath::Matrix matrix = mylib::GetMatrix_ScreenToWorld(
			screenWidth, screenHeight, view, projection
		);

		// nearプレーンのベクトルを作る
		DirectX::SimpleMath::Vector3 nearPointWorld = DirectX::SimpleMath::Vector3(
			static_cast<float>(mouseX),
			static_cast<float>(mouseY),
			0.0f
		);
		// farプレーンのベクトルを作る
		DirectX::SimpleMath::Vector3 farPointWorld  = DirectX::SimpleMath::Vector3(
			static_cast<float>(mouseX),
			static_cast<float>(mouseY),
			1.0f
		);

		// near/farをワールド座標に変換する
		nearPointWorld = DirectX::SimpleMath::Vector3::Transform(nearPointWorld, matrix);
		farPointWorld  = DirectX::SimpleMath::Vector3::Transform(farPointWorld,  matrix);

		// マウスレイの始点と進行方向ベクトルを計算する
		const DirectX::SimpleMath::Vector3 rayOrigin = nearPointWorld;
		const DirectX::SimpleMath::Vector3 rayDirection = farPointWorld - nearPointWorld;

		// マウスレイを作成する
		DirectX::SimpleMath::Ray ray(rayOrigin, rayDirection);

		return ray;
	}
}