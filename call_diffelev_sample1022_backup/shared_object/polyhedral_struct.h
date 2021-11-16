#pragma once

#include "vector_struct.h"

// 点データ配列のインデックスを示すデータ型
typedef int point_index_t;

// 多角形を表すデータ型
template < size_t number_of_vertex >
using poly_index = std::array<point_index_t, number_of_vertex>;

// 三角形データ
typedef poly_index<3> tria_index_t;

template < size_t dimension, size_t number_of_vertex >
struct polyhedral
{
public:
	std::vector< point<dimension> > verteces;
	std::vector< poly_index<number_of_vertex> > faces;
};

// 三角形で構成された3D多面体（非凸も含む）
typedef polyhedral<3, 3> polyhedral_tri3d_t;

