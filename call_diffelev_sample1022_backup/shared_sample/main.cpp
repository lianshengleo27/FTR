#include "args.h"

#include "read_pointcloud.h"
#include "read_e57.h"
#include "read_landxml.h"

#include "grid.h"
#include "diff.h"
#include "cut.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

int main(int argc, char* argv[])
{
  // 引数解析
  if(!args_analys(argc, argv)){
    print_help(std::cerr);
    return 1;
  }

  // 引数で指定があればヘルプを表示して終了
  if(arg_option::print_help){
    print_help(std::cout);
    return 0;
  }

  // 規格値等のパラメータをファイルから取得
  if(!arg_option::elev_cfg_json.empty()){
    if(!read_json_threshold_values(arg_option::elev_cfg_json.c_str(), arg_option::elev_cfg.thresholds)){
      std::cerr << "Read Standard Value JSON failure." << std::endl;
      return 1;
    }
  }
  if(!arg_option::out_cfg_json.empty()){
    if(!write_json_threshold_values(arg_option::out_cfg_json.c_str(), arg_option::elev_cfg.thresholds)){
      std::cerr << "WARNING: Write Used Standard Value JSON failure." << std::endl;
    }
  }

  // 各種オプション引数を表示
  std::cout << "Input Pointcloud :" << arg_option::input_pcl << std::endl;
  std::cout << "  Input Separator : \'" << (char)arg_option::read_cfg.separator << "\'" << std::endl;
  std::cout << "  Ignore HeaderLines : " << arg_option::read_cfg.header_line << std::endl;
  std::cout << "  Grid Origin : <" << arg_option::grid_cfg.grid_origin[0];
  std::cout << "," << arg_option::grid_cfg.grid_origin[1] << ">" << std::endl;
  std::cout << "  Grid Width : <" << arg_option::grid_cfg.grid_width[0];
  std::cout << "," << arg_option::grid_cfg.grid_width[1] << ">" << std::endl;
  std::cout << "Input LandXML :" << arg_option::input_landxml << std::endl;
  std::cout << "Output STL :" << arg_option::output_stl << std::endl;
  std::cout << "Calc Thin Pointcloud:" << (arg_option::thin_grid_flag ? "ON" : "OFF") << std::endl;
  if(arg_option::thin_grid_flag){
    std::cout << "  Output Pointcloud :" << arg_option::thinned_pcl << std::endl;
    std::cout << "  ThinGridMode:" << arg_option::thin_grid_mode << std::endl;
    std::cout << "  CutByPolygon:";
    if(arg_option::cut_polygon.empty()) std::cout << "OFF";
    for(auto p : arg_option::cut_polygon){
      std::cout << " <" << p[0] << "," << p[1] << ">";
    }
    std::cout << std::endl;
    std::cout << "  CutNearSurface:" << (arg_option::cut_far_surface_flag ? "ON" : "OFF");
    if(arg_option::cut_far_surface_flag){
      std::cout << " threshold:" << arg_option::cut_far_threshold;
      std::cout << " thread:" << arg_option::cut_far_thread_num;
    }
    std::cout << std::endl;
  }
  std::cout << "Calc Diff Elevation:" << (arg_option::diff_elev_flag ? "ON" : "OFF") << std::endl;
  if(arg_option::thin_grid_flag){
    std::cout << "  Output GridDiffCsv:" << arg_option::grid_diff_csv << std::endl;
    std::cout << "  Output DiffElevJson:" << arg_option::diff_elev_json << std::endl;
  }
  std::cout << "Calc Slope Angle:" << (arg_option::slope_angle_flag ? "ON" : "OFF") << std::endl;
  if(arg_option::thin_grid_flag){
    std::cout << "  Output SlopeAngleJson:" << arg_option::slope_angle_json << std::endl;
    std::cout << "  JudgeSlopeMode:" << arg_option::judge_slope_mode << std::endl;
  }
  std::cout << "Standard Values:" << std::endl;
  {
    const threshold_values& tv = arg_option::elev_cfg.thresholds;
    std::cout << "  Top/Slope Gradient:" << tv.top_slope_grad_threshold << std::endl;
    std::cout << "  Top Averate Difference:" << tv.top_average_threshold << std::endl;
    std::cout << "  Top Maximum Difference:" << tv.top_maximum_threshold << std::endl;
    std::cout << "  Top Minimum Difference:" << tv.top_minimum_threshold << std::endl;
    std::cout << "  Top Discard Rate:" << tv.top_discard_rate << std::endl;
    std::cout << "  Top Number of Data in m^2:" << tv.top_minimum_num_in_m2 << std::endl;
    std::cout << "  Slope Averate Difference:" << tv.slope_average_threshold << std::endl;
    std::cout << "  Slope Maximum Difference:" << tv.slope_maximum_threshold << std::endl;
    std::cout << "  Slope Minimum Difference:" << tv.slope_minimum_threshold << std::endl;
    std::cout << "  Slope Discard Rate:" << tv.slope_discard_rate << std::endl;
    std::cout << "  Slope Number of Data in m^2:" << tv.slope_minimum_num_in_m2 << std::endl;
    std::cout << "  Ignore Distance from Non-planar Edge:" << tv.ignore_distance << std::endl;
  }

  bool print_time = false;
  std::chrono::system_clock::time_point t_bgn, t_end;

  // 点群を読み込み
  std::vector<point3d_t> points;
  t_bgn = std::chrono::system_clock::now();
  std::cout << "Read pointcloud..." << std::endl;
  {
    if(arg_option::input_pcl.substr(arg_option::input_pcl.find_last_of('.')) == ".e57"){
      if(!read_pointcloud_e57(arg_option::input_pcl.c_str(), points)){
	std::cerr << "Read e57 pointcloud failure." << std::endl;
	return 1;
      }
    }else{
      if(!read_pcl::read_pointcloud_text(arg_option::input_pcl.c_str(), points, arg_option::read_cfg)){
	std::cerr << "Read pointcloud failure." << std::endl;
	return 1;
      }
    }
  }

  t_end = std::chrono::system_clock::now();
  std::chrono::duration<double, std::milli> time_ms = t_end - t_bgn;

  if(print_time){
    std::cout << "Read pointcloud Done. " << time_ms.count() << "[ms]"<< std::endl;
  }else{
    std::cout << "Read pointcloud Done." << std::endl;
  }

  polyhedral_tri3d_t poly;
  grid::grid_triangle_container grid_tria(arg_option::grid_cfg);
  if(!arg_option::input_landxml.empty()){
    t_bgn = std::chrono::system_clock::now();
    std::cout << "Read LandXML..." << std::endl;
    if (!read_xml::read_xml(arg_option::input_landxml.c_str(), poly)) {
      std::cerr << "Read LandXML failure." << std::endl;
      return 1;
    }

    if(!arg_option::output_stl.empty()){
      if(!read_xml::write_stl(arg_option::output_stl.c_str(), poly)){
	std::cerr << "Write STL failure." << std::endl;
	return 1;
      }
    }

    auto p = grid_tria.add_polyhedral(poly);
    if (p.first == -1 || p.second == -1) {
      return 1;
    }
    t_end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> time_ms = t_end - t_bgn;
    if(print_time){
      std::cout << "Read LandXML Done. " << time_ms.count() << "[ms]"<< std::endl;
    }else{
      std::cout << "Read LandXML Done." << std::endl;
    }
  }

  if(arg_option::thin_grid_flag){
    t_bgn = std::chrono::system_clock::now();
    std::cout << "Thin pointcloud..." << std::endl;

    // 指定多角形で点をカット
    if(!arg_option::cut_polygon.empty()){
      if(!check_out_of_polyhedral_region(arg_option::cut_polygon, grid_tria)){
	std::cerr << "WARNING RESULT: A part of Cut-polygon is out of designed region." << std::endl;
      }
      if(!extract_inner_polygon(points, arg_option::cut_polygon)){
	std::cerr << "Cut Points Outer Polygon failure." << std::endl;
	return 1;
      }
    }

    bool nopt_warn_flag = false;
    if(points.empty()){
      nopt_warn_flag = true;
      std::cerr << "WARNING RESULT: No points remained." << std::endl;
    }

    // グリッドに点群を格納
    grid::grid_container grid;
    if(!grid.add(points)){
      std::cerr << "Register Points to Grid failure." << std::endl;
      return 1;
    }
    points.clear();

    // 指定範囲外の点をカット
    if(arg_option::cut_far_surface_flag){
      if(!grid.remove_far_triangles(grid_tria, arg_option::cut_far_threshold, arg_option::cut_far_thread_num)){
	std::cerr << "Cut Points Far Surface failure." << std::endl;
	return 1;
      }
    }

    if(!nopt_warn_flag && grid.empty()){
      nopt_warn_flag = true;
      std::cerr << "WARNING RESULT: No points remained." << std::endl;
    }

    // 変化点に近い点をカット
    if(!grid.remove_near_nonplaner_edge(grid_tria, arg_option::elev_cfg.thresholds.ignore_distance)){
      std::cerr << "Cut Points Near non-Planer Edge failure." << std::endl;
      return 1;
    }

    if(!nopt_warn_flag && grid.empty()){
      nopt_warn_flag = true;
      std::cerr << "WARNING RESULT: No points remained." << std::endl;
    }

    // グリッドごとに間引き
    if(arg_option::thin_grid_mode == "maximum"){
      if(!grid.thin_max()){
	std::cerr << "Thinning Points of each Grid failure." << std::endl;
	return 1;
      }
    }else if(arg_option::thin_grid_mode == "minimum"){
      if(!grid.thin_min()){
	std::cerr << "Thinning Points of each Grid failure." << std::endl;
	return 1;
      }
    }else if(arg_option::thin_grid_mode == "median"){
      if(!grid.thin_median()){
	std::cerr << "Thinning Points of each Grid failure." << std::endl;
	return 1;
      }
    }else if(arg_option::thin_grid_mode == "mode"){
      if(!grid.thin_mode(arg_option::thin_mode_interval)){
	std::cerr << "Thinning Points of each Grid failure." << std::endl;
	return 1;
      }
    }

    if(!nopt_warn_flag && grid.empty()){
      nopt_warn_flag = true;
      std::cerr << "WARNING RESULT: No points remained." << std::endl;
    }

    // 間引き後の点をすべて取得
    std::vector<point3d_t> thinned;
    if(!grid.collect_all(thinned)){
      std::cerr << "Collecting Thinned Points failure." << std::endl;
      return 1;
    }

    // ファイルに出力
    std::ofstream ofs(arg_option::thinned_pcl.c_str());
    if(ofs.fail() || !ofs.is_open()){
      std::cerr << "Open Output File failure." << std::endl;
      return 1;
    }
    for(auto p : thinned){
      ofs << std::setprecision(15) << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }
    if(ofs.fail()){
      std::cerr << "Output File failure." << std::endl;
      return 1;
    }

    points = thinned;
    t_end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> time_ms = t_end - t_bgn;

    if(print_time){
      std::cout << "Thin pointcloud Done. " << time_ms.count() << "[ms]" << std::endl;
    }else{
      std::cout << "Thin pointcloud Done." << std::endl;
    }
  }

  if(!arg_option::input_landxml.empty()){
    diff_elevation_config diff_elev_cfg = arg_option::elev_cfg;
    if(arg_option::judge_slope_mode == "calculation"){
      diff_elev_cfg.judge = calculation;
    }else if(arg_option::judge_slope_mode == "force-top"){
      diff_elev_cfg.judge = force_top;
    }else if(arg_option::judge_slope_mode == "force-slope"){
      diff_elev_cfg.judge = force_slope;
    }

    if(arg_option::diff_elev_flag){
      t_bgn = std::chrono::system_clock::now();
      std::cout << "Calc Diff Elevation..." << std::endl;
      std::ofstream ofs(arg_option::diff_elev_json.c_str());
      if(ofs.fail() || !ofs.is_open()){
	std::cerr << "Open Output DiffElevJson File failure." << std::endl;
	return 1;
      }
      std::vector<diff_elevation_result> diffs;
      std::vector<diff_elevation_result> discarded;
      double area = arg_option::grid_cfg.grid_width[0] * arg_option::grid_cfg.grid_width[1];
      if (!calc_diff_elevation(diff_elev_cfg, area, points, grid_tria, diffs, discarded, ofs)) {
	std::cerr << "WARNING RESULT: Out of Standard" << std::endl;
      }
      // 棄却点も含めて出力する
      std::copy(discarded.begin(), discarded.end(), std::back_inserter(diffs));

      if(!write_calc_result(arg_option::grid_diff_csv.c_str(), points, grid_tria.get_config(), diffs)){
	std::cerr << "Write Diff Elevation Result failure." << std::endl;
	return 1;
      }

      t_end = std::chrono::system_clock::now();
      std::chrono::duration<double, std::milli> time_ms = t_end - t_bgn;
      if(print_time){
	std::cout << "Calc Diff Elevation Done. " << time_ms.count() << "[ms]" << std::endl;
      }else{
	std::cout << "Calc Diff Elevation Done." << std::endl;
      }
    }  

    if(arg_option::slope_angle_flag){
      t_bgn = std::chrono::system_clock::now();
      std::cout << "Calc Slope Angle..." << std::endl;
      std::ofstream ofs(arg_option::slope_angle_json.c_str());
      if(ofs.fail() || !ofs.is_open()){
	std::cerr << "Open Output SlopeAngleJson File failure." << std::endl;
	return 1;
      }
      if(!calc_slope_angle(diff_elev_cfg, points, grid_tria, ofs)){
	std::cerr << "Calc Slope Angle failure." << std::endl;
	return 1;
      }
      t_end = std::chrono::system_clock::now();
      std::chrono::duration<double, std::milli> time_ms = t_end - t_bgn;
      if(print_time){
	std::cout << "Calc Slope Angle Done. " << time_ms.count() << "[ms]" << std::endl;
      }else{
	std::cout << "Calc Slope Angle Done." << std::endl;
      }
    }
  }

  std::cout << "All Done" << std::endl;
  return 0;
}


