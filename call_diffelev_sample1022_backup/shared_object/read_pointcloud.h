#pragma once

#include <vector>
#include <string>

#include "vector_struct.h"

namespace read_pcl {
	// 読込設定用の構造体
	// ファイルの形式に合わせて読み方を変えたい場合に変更する
	struct read_text_config
	{
		// ヘッダの行数(無視する行数)
		// 点データが途中の行から始まるような場合に設定する
		size_t header_line = 0;

		// 区切り文字
		// 初期値ではスペース区切り(例："1.0323 2.35365 3.1525")に対応する
		// 他にカンマ区切り(例:"3.2356,5.5662,2.6674")などがあり得る
		char separator = ',';
	};

	bool read_pointcloud_text(const char* file_path, std::vector<point3d_t>& read_points, const read_text_config config = read_text_config());

	bool read_pointcloud_text(const char* file_path, void(*line_reader)(const std::vector<std::string>& columns), const read_text_config config = read_text_config());
	bool write_pointcloud_text(const char* file_path, const std::vector<point3d_t>& write_points);

}
