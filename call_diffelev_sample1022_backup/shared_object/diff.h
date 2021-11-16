#pragma once

#include "read_landxml.h"
#include "vector_struct.h"
#include "grid.h"

enum judge_top_slope
{
	calculation,
	force_top,
	force_slope
};

struct threshold_values
{
  double top_slope_grad_threshold = 0.06; // 天面か法面かの判断のしきい値
  double top_average_threshold = 0.05; // 天面との点群較差の平均値の規格値
  double top_maximum_threshold = 0.150; // 天面との点群較差の最大値の規格値
  double top_minimum_threshold = 0.150; // 天面との点群較差の最小値の規格値
  double top_discard_rate = 0.003; // 天面との点群較差の棄却点数の規格値
  int top_minimum_num_in_m2 = 1; // 天面との点群較差の1m^2におけるデータ数の規格値
  double slope_average_threshold = 0.08; // 法面との点群較差の平均値の規格値
  double slope_maximum_threshold = 0.190; // 法面との点群較差の最大値の規格値
  double slope_minimum_threshold = 0.190; // 法面との点群較差の最小値の規格値
  double slope_discard_rate = 0.003; // 法面との点群較差の棄却点数の規格値
  int slope_minimum_num_in_m2 = 1; // 法面との点群較差の1m^2におけるデータ数の規格値
  double ignore_distance = 0.050; // 設計データの変化点付近で評価から外す距離
};

struct diff_elevation_config
{
  enum judge_top_slope judge;
  threshold_values thresholds;
};

struct diff_elevation_result
{
  int point_index;
  double tria_height;
  double diff_value;
  double diff_rate;
};

// 規格値データの外部からの取り込み
// filepath[in]: 規格値データがjson形式で書かれたファイルのパス
// values[out]: 取り込み結果の規格値データ
// ファイルの形式はそれぞれの変数名をキーとしたjson形式
// ファイル内にキーがなければデフォルト値が使用される
// 例:
// {
//   "top_slope_grad_threshold":0.10,
//   "top_average_threshold":0.03
// }
//
// => top_slope_grad_threshold(天面か法面かの判断の閾値)を0.10
//    top_average_threshold(天面との点群較差の平均値の規格値)を0.03
//    に指定して残りはデフォルト値(上記構造体定義の=の右側の値)を使用する
bool read_json_threshold_values(const char* filepath, threshold_values& values);
bool write_json_threshold_values(const char* filepath, const threshold_values& values);

// 点群データを設計データの面ごとに分類
// de_cfg[in]: 規格値等の算出に関するパラメータをまとめた構造体を入力
// pcl[in]: 評価用点群データの列を入力
// gtc[in]: 設計データをグリッドごとに区切ったデータを入力
// divided[out]: 評価用点群データを面ごとに分類したものを出力
// 成功すればtrue, 何かしら失敗すればfalseが返る
bool calc_divide_by_planes(const diff_elevation_config& de_cfg, std::vector<point3d_t>& pcl, const grid::grid_triangle_container& gtc, std::vector< std::vector<point3d_t> >& divided);

// 法角度の算出
// de_cfg[in]: 規格値等の算出に関するパラメータをまとめた構造体を入力
//            de_cfg.judgeで法面かどうかの算出を行うかを指定する
//            force_topは意味なし
//            force_slopeは入力点が全て1つの法面だと仮定して計算する
//            calculationは設計データとの位置比較により分類後、設計データが法面のものに対してそれぞれ1つずつの平面として計算する
// pcl[in]: 評価用点群データの列を入力
// gtc[in]: 設計データをグリッドごとに区切ったデータを入力
// out[in/out]: 結果の法角度データをjson形式で出力する先
// 成功すればtrue, 何かしら失敗すればfalseが返る
bool calc_slope_angle(const diff_elevation_config& de_cfg, const std::vector<point3d_t>& pcl, const grid::grid_triangle_container& gtc, std::ostream& out);

// 標高較差の算出
// de_cfg[in]: 規格値等の算出に関するパラメータをまとめた構造体を入力
// pcl[in]: 評価用点群データの列を入力
// gtc[in]: 設計データをグリッドごとに区切ったデータを入力
// diffs[out]: 較差データを出力
// discarded[out]: 棄却点のデータを出力
// out[in/out]: 算出結果の全体的な評価値をjson形式で出力する先
// 全体評価が規格値以内に収まればtrue,規格値外となるものがあればfalseを返す
bool calc_diff_elevation(const diff_elevation_config& de_cfg, double grid_area, std::vector<point3d_t>& pcl, const grid::grid_triangle_container& gtc, std::vector<diff_elevation_result>& diffs, std::vector<diff_elevation_result>& discarded, std::ostream& out);

// 標高較差データの出力
// de_cfg[in]: 規格値等の算出に関するパラメータをまとめた構造体を入力
// file_path[in]: 出力先ファイル名を入力
// pcl[in]: 評価用点群データの列を入力
// cfg[in]: グリッドの設定データを入力
// diffs[out]: 較差データを入力
// 成功すればtrue, 何かしら失敗すればfalseが返る
bool write_calc_result(const char* file_path, const std::vector<point3d_t>& pcl, const grid::grid_config& cfg, const std::vector<diff_elevation_result>& diffs);
