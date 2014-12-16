#include "StdAfx.h"
#include "Layout.h"
#include <stdio.h>
#include <vector>

using namespace std;

#pragma warning( disable:4996 )
std::vector<CPreview> CLayout::ReadConfig(HWND my_hwnd,std::string src_file )
{
	std::vector<CPreview> previews;

	char message[256];

	int x =0,y=0,width=0,height=0, tgt_x=0, tgt_y=0;
	FILE* f = fopen( src_file.c_str(), "r" );
	// first line = name of the 
	do {
		fgets(message, 256,f );
		int i = strlen(message)-1;
		while (message[i] == 13 || message[i] == 10 )
		{
			message[i] = 0;
			i--;
		}
		
		do {
			fscanf(f, "%d,%d,%d,%d=>%d,%d\r\n",&x,&y,&width,&height, &tgt_x, &tgt_y);
			if (x + y + width + height > 0)
			{
				HWND hwnd = FindWindow(NULL,message);
				if (!hwnd)
					continue;
				width-= x;
				height -=y;

				RECT src;
				src.left = x;
				src.top = y;
				src.right = x + width;
				src.bottom =  y + height;

				RECT tgt;
				tgt.left = tgt_x;
				tgt.top = tgt_y;
				tgt.right = tgt_x + width;
				tgt.bottom = tgt_y + height;

				CPreview preview;
				preview.Init( message, my_hwnd, hwnd, src, tgt);
				previews.push_back(preview);
			}

		} while (x + y + width + height > 0);
	} while (!feof(f));
	fclose(f);
	return previews;
}

void CLayout::SaveConfig( std::string dst_file , std::vector<CPreview> config )
{
	
	int x =0,y=0,width=0,height=0, tgt_x=0, tgt_y=0;
	FILE* f = fopen( dst_file.c_str(), "w" );
	// first line = name of the 
	for (unsigned i = 0 ;i < config.size() ; i ++ )
	{
		CPreview& preview = config[i];
		fprintf(f, "%s\r\n", preview.GetName().c_str());
		RECT src = preview.GetSrcRect();
		RECT dst = preview.GetDstRect();
		fprintf(f, "%d,%d,%d,%d=>%d,%d\r\n", src.left, src.top, src.right, src.bottom, dst.left , dst.top);
		fprintf(f, "0,0,0,0=>0,0\r\n");
	}
	fclose(f);
}
