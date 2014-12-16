#include "StdAfx.h"
#include "Preview.h"


CPreview::CPreview() :
	m_HWND(0), m_Thumb(0), m_source_rect(), m_target_rect()
{
}


CPreview::~CPreview(void)
{

}

void CPreview::Draw()  const
{
	DWM_THUMBNAIL_PROPERTIES dskThumbProps;
    dskThumbProps.dwFlags =  DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE ;
    dskThumbProps.fSourceClientAreaOnly = FALSE; 
    dskThumbProps.fVisible = TRUE;
    dskThumbProps.opacity = 255;
    dskThumbProps.rcDestination = m_target_rect;
	dskThumbProps.rcSource = m_source_rect;
    // Display the thumbnail
    DwmUpdateThumbnailProperties(m_Thumb,&dskThumbProps);
}

bool CPreview::Init(const char* name, HWND my_hwnd, HWND target_window, RECT source_rect, RECT target_rect)
{
	DwmRegisterThumbnail(my_hwnd, target_window, &m_Thumb);
	m_source_rect = source_rect;
	m_target_rect = target_rect;
	m_HWND = target_window;
	m_MyWND = my_hwnd;
	m_name = name;
	return true;
}

bool Contains(RECT container, RECT containee)
{
	bool in_x = container.left <= containee.left && (container.right >= containee.right);
	bool in_y = container.top <= containee.top && (container.bottom >= containee.bottom);
	return in_x && in_y;
}

bool  CPreview::AcceptMouse( UINT message,  WPARAM wParam, LPARAM lParam) 
{
	
	int  xPos = GET_X_LPARAM(lParam); 
	int  yPos = GET_Y_LPARAM(lParam); 
	bool b_in_rect_x = m_target_rect.left < xPos  && m_target_rect.right > xPos;
	bool b_in_rect_y = m_target_rect.top < yPos  && m_target_rect.bottom > yPos;
    m_active = false;
	if (b_in_rect_x && b_in_rect_y)
	{
		xPos = xPos - m_target_rect.left + m_source_rect.left -8;
		yPos = yPos - m_target_rect.top + m_source_rect.top -8;
		lParam = MAKELPARAM(xPos, yPos);
		RECT wnd_rect,target_wnd_rect;
		GetWindowRect(m_MyWND, &wnd_rect);
		GetWindowRect(m_HWND, &target_wnd_rect);
		wnd_rect.left += target_wnd_rect.left;
		wnd_rect.right = wnd_rect.left + target_wnd_rect.right;
		wnd_rect.top  = wnd_rect.top + target_wnd_rect.top;
		wnd_rect.bottom = wnd_rect.top  + target_wnd_rect.bottom;
		if (!Contains(target_wnd_rect, wnd_rect))
		{
			GetWindowRect(m_MyWND, &wnd_rect);
			SetWindowPos(m_HWND, HWND_BOTTOM ,wnd_rect.left + m_target_rect.left, wnd_rect.top + m_target_rect.top, 0,0,SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
		}
		PostMessage(m_HWND, message, wParam, lParam);
		m_active = true;
	}
	return m_active;
}

void  CPreview::AcceptKeyboard( UINT message,  WPARAM wParam, LPARAM lParam) 
{
	if (m_active) 
	{
		if (message == WM_KEYUP || message == WM_KEYDOWN) 
			PostMessage( m_HWND, message, wParam, lParam);
	}
}

void CPreview::Move( int dx, int dy )
{
	m_target_rect.left += dx;
	m_target_rect.right += dx;
	m_target_rect.top += dy;
	m_target_rect.bottom += dy;
	PostMessage(m_MyWND, WM_PAINT, 0,0);
}

bool CPreview::IsActive( LPARAM lParam )
{
	int  xPos = GET_X_LPARAM(lParam); 
	int  yPos = GET_Y_LPARAM(lParam); 
	bool b_in_rect_x = m_target_rect.left < xPos  && m_target_rect.right > xPos;
	bool b_in_rect_y = m_target_rect.top < yPos  && m_target_rect.bottom > yPos;
	if (b_in_rect_x && b_in_rect_y)
	{
		return true;
	}
	return false;
}

RECT CPreview::GetSrcRect()
{
	return m_source_rect;
}

RECT CPreview::GetDstRect()
{
	return m_target_rect;
}
