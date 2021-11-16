#pragma once

#include <array>
#include <vector>

// 座標値を表すデータ型
template < size_t dim >
using point = std::array<double, dim>;

// 2D座標値データ
typedef point<2> point2d_t;

// 3D座標値データ
typedef point<3> point3d_t;

// 相対ベクトルを表すデータ型
template < size_t dim >
using vector = std::array<double, dim>;

// 2D相対ベクトルデータ
typedef vector<2> vector2d_t;

// 3D相対ベクトルデータ
typedef vector<3> vector3d_t;

// 幅・大きさを表すデータ型
template < size_t dim >
using size = std::array<double, dim>;

// 2D幅データ
typedef size<2> size2d_t;

// 3D幅データ
typedef size<3> size3d_t;
