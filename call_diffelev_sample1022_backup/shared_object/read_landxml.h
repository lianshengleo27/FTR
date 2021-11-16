#pragma once

#include <array>
#include <vector>
#include <string>

#include "polyhedral_struct.h"

namespace read_xml {
	// 読込関数
	bool read_xml(const char* file_path, polyhedral_tri3d_t& read_poly);
  size_t get_id(size_t idx);

	bool write_stl(const char* file_path, const polyhedral_tri3d_t& write_poly);
}
