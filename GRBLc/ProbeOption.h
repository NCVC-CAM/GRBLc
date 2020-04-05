#pragma once

enum {
	probI_ZpFeed=0,
	probI_ZpSetMethod,
	probI_ZpWork,
	probI_XYFeed,
	probI_XYWork,
		probI_MAX	//	[5]
};
enum {
	probF_ZpLength=0,
	probF_ZpThickness,
	probF_ZpDistance,
	probF_XYLength,
	probF_XYDistance,
		probF_MAX	// [5]
};

class CProbeZ;
class CProbeXY;

/////////////////////////////////////////////////////////////////////
// CProbeOption

class CProbeOption
{
	friend class CProbeZ;
	friend class CProbeXY;

	union {
		struct {
			int		m_nZpFeed,
					m_nZpSetMethod,
					m_nZpWork,
					m_nXYFeed,
					m_nXYWork;
		};
		int			m_intOption[probI_MAX];
	};
	union {
		struct {
			float	m_fZpLength,
					m_fZpThickness,
					m_fZpDistance,
					m_fXYLength,
					m_fXYDistance;
		};
		float		m_fltOption[probF_MAX];
	};

	void	DefaultSetting(void);

public:
	CProbeOption();

	bool	Read(void);
	bool	Write(void);
};
