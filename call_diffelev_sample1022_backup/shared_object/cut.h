#pragma once

#include "read_landxml.h"
#include "vector_struct.h"
#include "polyhedral_struct.h"
#include "grid.h"

// 点群データから多角形で指定された領域内の点を抽出する
// pcl[in/out]: 点群データの列を入力、計算の結果として抽出されたモノのみが残る
// poly[in]: 多角形を点の列で指定する Zは無視してXY平面上の多角形として入力する
// 成功すればtrue, 何かしら失敗すればfalseが返る
bool extract_inner_polygon(std::vector<point3d_t>& pcl, const std::vector<point2d_t>& poly);

// 多角形の頂点が設計データからなる領域の外側にないかをチェックする
// poly[in]: 多角形を点の列で指定する Zは無視してXY平面上の多角形として入力する
// gtc[in]: 設計データをグリッドごとに区切ったデータを入力
// 全て内部にあればtrue, 1点でも外側にあればfalseが返る
bool check_out_of_polyhedral_region(const std::vector<point2d_t>& poly, const grid::grid_triangle_container& gtc);

// 点群データから設計データに近い点を抽出する
// pcl[in/out]: 点群データの列を入力、計算の結果として抽出されたモノのみが残る
// gtc[in]: 設計データをグリッドごとに区切ったデータを入力
// threshold[in]: 設計データとの距離のしきい値 この値以下の距離の点が残る
// 成功すればtrue, 何かしら失敗すればfalseが返る
bool extract_near_triangles(std::vector<point3d_t>& pcl, const grid::grid_triangle_container& gtc, double threshold, size_t thread_num);

