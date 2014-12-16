#pragma once

#include <vector>
#include "Preview.h"
using namespace std;

class CLayout
{
public:
	std::vector<CPreview> ReadConfig(HWND my_window, std::string src_file);
	void SaveConfig( std::string dst_file , std::vector<CPreview> config);
};

