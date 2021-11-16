#include "args.h"

#include <cstring>

#include <queue>
#include <map>
#include <string>
#include <filesystem>

namespace arg_option{
  std::string input_pcl;
  std::string input_landxml;
  std::string output_stl;
  std::string thinned_pcl;
  std::string grid_diff_csv;
  std::string diff_elev_json;
  std::string slope_angle_json;
  read_pcl::read_text_config read_cfg;
  grid::grid_config grid_cfg;
  std::string elev_cfg_json;
  diff_elevation_config elev_cfg;
  std::string out_cfg_json;
  std::string thin_grid_mode = "none";
  double thin_mode_interval = 0.01;
  std::vector<point2d_t> cut_polygon;
  bool cut_far_surface_flag = false;
  double cut_far_threshold = 0.10;
  size_t cut_far_thread_num = 1;
  std::string judge_slope_mode = "calculation";
  bool print_help = false;
  bool thin_grid_flag = false;
  bool cut_polygon_flag = false;
  bool cut_far_flag = false;
  bool diff_elev_flag = false;
  bool slope_angle_flag = false;
}

// ヘルプ表示
void print_help(std::ostream& ost)
{
  ost << "call_diffelev_sample" << std::endl;
  ost << "  標高較差算出プログラムのサンプルです" << std::endl;
  ost << std::endl;
  ost << "Usage:" << std::endl;
  ost << "  call_diffelev_sample [OPTIONS]" << std::endl;
  ost << std::endl;
  ost << "Options:" << std::endl;
  ost << "  -p, --input-pcl       入力点群ファイルを指定します" << std::endl;
  ost << "  -g, --thin-grid       入力点群をグリッド毎に間引きます 計算方法を指定します" << std::endl;
  ost << "                        \"maximum\":グリッドのZ座標が最大値の点を残します" << std::endl;
  ost << "                        \"minimum\":グリッドのZ座標が最小値の点を残します" << std::endl;
  ost << "                        \"median\":グリッドのZ座標が中央値となる点を残します" << std::endl;
  ost << "                        \"mode\":グリッドのZ座標でヒストグラムを取った際の最瀕値となる点を残します 続けてヒストグラムの間隔を指定してください" << std::endl;
  ost << "                        \"none\":間引きを行いません" << std::endl;
  ost << "  -c, --cut-polygon     多角形を指定し、点群データからその内側にある点のみを抽出して残りを消去します" << std::endl;
  ost << "                        最初に多角形の点の数を指定し、続けて1点目のX座標、1点目のY座標、2点目以降...と指定します" << std::endl;
  ost << "  -n, --extract-near    設計データから近い点のみを抽出して残りを消去します" << std::endl;
  ost << "  -t, --thinned-pcl     間引かれたデータの出力先を指定します" << std::endl;
  ost << "  -o, --grid-origin     グリッドの基準位置を数値2つスペース区切りで設定します(初期値:0 0)" << std::endl;
  ost << "  -w, --grid-width      グリッドの感覚を数値2つスペース区切りで設定します(初期値:1 1)" << std::endl;
  ost << "  -s, --separator       入力ファイルの区切り文字を設定します(初期値:',')" << std::endl;
  ost << "  -i, --ignore-lines    入力ファイルの先頭何行を無視するかを指定します(初期値:0)" << std::endl;
  ost << "  -x, --input-landxml   設計データとなるLandXmlファイルを指定します" << std::endl;
  ost << "  -l, --output-stl      設計データから得られた表面形状を指定したファイルにSTL形式で出力します" << std::endl;
  ost << "  -d, --diff-elevation  較差を計算し、指定したファイルに計算結果を出力します" << std::endl;
  ost << "                        各グリッドの較差算出結果のcsvファイル、全体の評価結果のjsonファイルの順に指定します" << std::endl;
  ost << "  -v, --standard-value  較差の計算で使用する規格値等のパラメータが書かれたjsonファイルを指定します" << std::endl;
  ost << "  -u, --out-used-value  較差の計算で使用した規格値等のパラメータが書かれたjsonファイルの出力先を指定します" << std::endl;
  ost << "  -a, --slope-angle     法角度を計算し、指定したファイルに計算結果を出力します" << std::endl;
  ost << "  -j, --judge-slope     評価する点群が天面か法面かを判定する計算方法を指定します" << std::endl;
  ost << "                        \"force-top\":必ず天面として計算します" << std::endl;
  ost << "                        \"force-slope\":必ず法面として計算します" << std::endl;
  ost << "                        \"calculation\":近接の設計データを計算で求め、その傾きで天面か法面かを決定します" << std::endl;
  ost << "  -h, --help            このヘルプを表示します" << std::endl;
}

// 引数解析
bool args_analys(int argc, char* argv[])
{
  // 取り込んだオプション指定を一文字指定の文字で格納する
  std::queue<char> option_symbols;
  // オプション指定なしの引数を格納する
  std::vector<std::string> no_opt;

  // 長い指定と一文字指定の対応付け
  std::map<std::string, char> long2short;
  long2short.insert(std::make_pair("input-pcl", 'p'));
  long2short.insert(std::make_pair("thin-grid", 'g'));
  long2short.insert(std::make_pair("cut-polygon", 'c'));
  long2short.insert(std::make_pair("extract-near", 'n'));
  long2short.insert(std::make_pair("thinned-pcl", 't'));
  long2short.insert(std::make_pair("grid-origin", 'o'));
  long2short.insert(std::make_pair("grid-width", 'w'));
  long2short.insert(std::make_pair("separator", 's'));
  long2short.insert(std::make_pair("ignore-lines", 'i'));
  long2short.insert(std::make_pair("input-landxml", 'x'));
  long2short.insert(std::make_pair("output-stl", 'l'));
  long2short.insert(std::make_pair("diff-elevation", 'd'));
  long2short.insert(std::make_pair("standard-value", 'v'));
  long2short.insert(std::make_pair("out-used-value", 'u'));
  long2short.insert(std::make_pair("slope-angle", 'a'));
  long2short.insert(std::make_pair("judge-slope", 'j'));
  long2short.insert(std::make_pair("help", 'h'));

  for (int i = 1; i < argc; ++i) {
    if (!option_symbols.empty()) { // オプション指定時
      char op_symbol = option_symbols.front();
      option_symbols.pop();

      switch (op_symbol) {
      case 'p': // Input Pointcloud
	arg_option::input_pcl = argv[i];
	break;
      case 'x': // Input LandXML
	arg_option::input_landxml = argv[i];
	break;
      case 'l': // Output STL
	arg_option::output_stl = argv[i];
	break;
      case 'g': // Thin-Grid
	arg_option::thin_grid_flag = true;
	arg_option::thin_grid_mode = argv[i];
	if(argv[i] == "mode"){
	  if (i + 1 < argc) {
	    arg_option::thin_mode_interval = std::stod(argv[i+1]);
	    i += 1;
	  }
	  else {
	    std::cerr << "Invalid Arguments of \'ThinGrid\'. Need 2 values if \"mode\" specified." << std::endl;
	    return false;
	  }
	}
	break;
      case 'c': // Cut by polygon
	arg_option::thin_grid_flag = true;
	{
	  int size = std::stoi(argv[i]);
	  if(size < 3){
	    std::cerr << "Invalid Arguments of \'CutPolygon\'. At least 3 points needed." << std::endl;
	    false;
	  }
	  if(i+size*2 >= argc){
	    std::cerr << "Invalid Arguments of \'CutPolygon\'. Too few points specified." << std::endl;
	    false;
	  }
	  for(int j = 0; j < size; ++j){
	    double x = std::stod(argv[i+j*2+1]);
	    double y = std::stod(argv[i+j*2+2]);
	    arg_option::cut_polygon.push_back(point2d_t({x,y}));
	  }
	  i += size*2;
	}
	break;
      case 'n': // CutNearSurface
	if(argv[i][0] != '-'){
	  arg_option::cut_far_threshold = std::stod(argv[i]);
	  if(i + 1 < argc && argv[i+1][0] != '-'){
	    arg_option::cut_far_thread_num = std::stoi(argv[i+1]);
	    i += 1;
	  }
	}
	break;
      case 't': // Thinned
	arg_option::thinned_pcl = argv[i];
	break;	
      case 's': // Column Separator
	arg_option::read_cfg.separator = argv[i][0];
	break;
      case 'i': // Ignore Header Lines
	arg_option::read_cfg.header_line = std::stoi(argv[i]);
	break;
      case 'o': // Grid Origin
	if (i + 1 < argc) {
	  arg_option::grid_cfg.grid_origin[0] = std::stod(argv[i]);
	  arg_option::grid_cfg.grid_origin[1] = std::stod(argv[i+1]);
	  i += 1;
	}
	else {
	  std::cerr << "Invalid Arguments of \'Grid Origin\'. Need 2 values." << std::endl;
	  return false;
	}
	break;
      case 'w': // Grid Width
	if (i + 1 < argc) {
	  arg_option::grid_cfg.grid_width[0] = std::stod(argv[i]);
	  arg_option::grid_cfg.grid_width[1] = std::stod(argv[i+1]);
	  i += 1;
	}
	else {
	  std::cerr << "Invalid Arguments of \'Grid Width\'. Need 2 values." << std::endl;
	  return false;
	}
	break;
      case 'd': // Diff Elevation
	if (i + 1 < argc) {
	  arg_option::grid_diff_csv = argv[i];
	  arg_option::diff_elev_json = argv[i+1];
	  arg_option::diff_elev_flag = true;
	  i += 1;
	}
	else {
	  std::cerr << "Invalid Arguments of \'Diff Elevation\'. Need 2 values." << std::endl;
	  return false;
	}
	break;
      case 'v': // StandardValue
	arg_option::elev_cfg_json = argv[i];
	break;
      case 'u': // OutputUsedStandardValue
	arg_option::out_cfg_json = argv[i];
	break;
      case 'a': // Slope Angle
	arg_option::slope_angle_json = argv[i];
	arg_option::slope_angle_flag = true;
	break;
      case 'j': // Judge Slope Mode
	arg_option::judge_slope_mode = argv[i];
	break;
      }
      continue;
    }

    if (argv[i][0] == '-') { // オプション指定
      if (argv[i][1] != '-') {
	// 一文字指定
	size_t len = strlen(argv[i]);
	for (size_t j = 1; j < len; ++j) {
	  option_symbols.push(argv[i][j]);
	}
      }
      else {
	// 長い指定
	std::string option(std::string(argv[i]).substr(2));
	std::map<std::string, char>::const_iterator f = long2short.find(option);
	if(f != long2short.end()){
	  option_symbols.push(f->second);
	}
	else{
	  std::cerr << "Unknown Option Name: " << option << std::endl;
	  return false;
	}
      }
    }
    else {
      no_opt.push_back(argv[i]);
    }

    if (!option_symbols.empty()) {
      switch (option_symbols.front()) {
      case 'h': // Help
	arg_option::print_help = true;
	option_symbols.pop();
	break;
      case 'n': // CutNearSurface オプション指定子の後に何もないか他のオプションが直後に来た場合、フラグだけ成立させて値はデフォルト値を使用する
	arg_option::thin_grid_flag = true;
	arg_option::cut_far_surface_flag = true;
	if(i+1 >= argc || argv[i+1][0] == '-'){
	  option_symbols.pop();
	}
	break;
      }
    }
  }

  for (auto arg : no_opt) {
    if (arg_option::input_pcl.empty()) {
      arg_option::input_pcl = arg;
    }
    else if (arg_option::thinned_pcl.empty()) {
      arg_option::thinned_pcl = arg;
    }
  }

  if(arg_option::input_pcl.empty()){
    std::cerr << "No Input Pointcloud Specified" << std::endl;
    return false;
  }
  if(arg_option::thin_grid_flag){
    if(arg_option::thinned_pcl.empty()){
      std::filesystem::path input(arg_option::input_pcl);
      arg_option::thinned_pcl = input.stem().string() + "_thinned" + input.extension().string();
    }
    if(arg_option::thin_grid_mode.empty()){
      std::cerr << "No Input ThinGridMode Specified" << std::endl;
      return false;
    }
    if(arg_option::cut_far_surface_flag){
      if(arg_option::input_landxml.empty()){
	std::cerr << "No Input LandXML file Specified" << std::endl;
	return false;
      }
    }
  }

  if(arg_option::diff_elev_flag){
    if(arg_option::input_landxml.empty()){
      std::cerr << "No Input LandXML file Specified" << std::endl;
      return false;
    }
    if(arg_option::grid_diff_csv.empty()){
      std::cerr << "No Output GridDiffCsv Specified" << std::endl;
      return false;
    }
    if(arg_option::diff_elev_json.empty()){
      std::cerr << "No Output DiffElevJson Specified" << std::endl;
      return false;
    }
  }

  if(arg_option::slope_angle_flag){
    if(arg_option::input_landxml.empty()){
      std::cerr << "No Input LandXML file Specified" << std::endl;
      return false;
    }
    if(arg_option::slope_angle_json.empty()){
      std::cerr << "No Output SlopeAngleJson Specified" << std::endl;
      return false;
    }
  }
  return true;
}

