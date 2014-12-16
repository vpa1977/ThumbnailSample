#pragma once

#include <dwmapi.h>
#include <vector>

class CPreview
{
public:
	CPreview();

	virtual ~CPreview(void);

	bool Init(const char* name , HWND source_window,HWND target_window, RECT source_rect, RECT target_rect);
	void Draw() const;
	bool AcceptMouse( UINT message,  WPARAM wParam, LPARAM lParam) ;
	void AcceptKeyboard( UINT message, WPARAM wParam, LPARAM lParam) ;
	void Move( int dx, int dy );
	bool IsActive( LPARAM lParam );
	std::string GetName() { return m_name; }
	RECT GetSrcRect();
	RECT GetDstRect();
private:
	std::string m_name;
	RECT m_source_rect;
	RECT m_target_rect;
	HWND m_HWND;
	HWND m_MyWND;
	HTHUMBNAIL m_Thumb;
	bool m_active;
};

