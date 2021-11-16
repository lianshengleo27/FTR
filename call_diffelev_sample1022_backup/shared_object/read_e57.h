#ifndef __READ_E57_HEADER_INCLUDED_B87F3EEBEF7C41AF95371C3E730D42F5__
#define __READ_E57_HEADER_INCLUDED_B87F3EEBEF7C41AF95371C3E730D42F5__

#include <array>
#include <vector>

typedef std::array<double, 3> point3d_t;

bool read_pointcloud_e57(const char* file_path, std::vector<point3d_t>& read_points);

#endif
