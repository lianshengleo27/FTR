#include "read_pointcloud.h"
#include "grid.h"
#include "diff.h"

#include <string>
#include <iostream>

namespace arg_option {
  extern std::string input_pcl;
  extern std::string input_landxml;
  extern std::string output_stl;
  extern std::string thinned_pcl;
  extern std::string grid_diff_csv;
  extern std::vector<point2d_t> cut_polygon;
  extern bool cut_far_surface_flag;
  extern double cut_far_threshold;
  extern size_t cut_far_thread_num;
  extern std::string diff_elev_json;
  extern std::string slope_angle_json;
  extern read_pcl::read_text_config read_cfg;
  extern grid::grid_config grid_cfg;
  extern std::string elev_cfg_json;
  extern diff_elevation_config elev_cfg;
  extern std::string out_cfg_json;
  extern std::string thin_grid_mode;
  extern double thin_mode_interval;
  extern std::string judge_slope_mode;
  extern bool print_help;
  extern bool thin_grid_flag;
  extern bool diff_elev_flag;
  extern bool slope_angle_flag;
}

bool args_analys(int argc, char* argv[]);
void print_help(std::ostream& ost);


