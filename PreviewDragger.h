#ifndef PREVIEW_DRAGGER_HPP

#define PREVIEW_DRAGGER_HPP
#include "Preview.h"


class PreviewDragger
{
public:
	PreviewDragger(void);
	virtual ~PreviewDragger(void);
	void SetActivePreview(CPreview * p_preview) 
	{ 
		m_pview =p_preview; 
		m_old_x = -1;
		m_old_y = -1;
	}
	void ProcessMove( int x , int y) 
	{
		if (m_pview)
		{
			if (m_old_x + m_old_y >= 0)
			{
				int dx = x - m_old_x;
				int dy = y - m_old_y;
				m_pview->Move(dx,dy);
			}
			m_old_x =x;
			m_old_y =y;
		}
	}
private:
	CPreview* m_pview;
	int m_old_x;
	int m_old_y;

};

#endif 

