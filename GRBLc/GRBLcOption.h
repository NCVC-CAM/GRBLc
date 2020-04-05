#pragma once

#include <string>
#include <bitset>
#include "boost/array.hpp"

enum {
	grblI_AutoConnect=0,
	grblI_WithTrace,
	grblI_QueryTime,
	grblI_Baudrate,
	grblI_JogFeed,
	grblI_WPos,
		grblI_MAX	//	[6]
};
enum {
	grblF_JogStep=0,
		grblF_MAX	// [1]
};
enum {
	grblS_CustomCmd1=0,
	grblS_CustomCmd2,
	grblS_CustomCmd3,
	grblS_Comport,
		grblS_MAX	// [4]
};

class CGRBLcSetup;

/////////////////////////////////////////////////////////////////////
// CGRBLcOption

class CGRBLcOption
{
	friend class CGRBLcSetup;

	union {
		struct {
			int		m_bAutoConnect,
					m_bWithTrace,
					m_nQueryTime,
					m_nBaudrate,
					m_nJogFeed,
					m_nWPos;
		};
		int			m_intOption[grblI_MAX];
	};
	union {
		struct {
			float	m_fJogStep;
		};
		float		m_fltOption[grblF_MAX];
	};
	boost::array<std::string, grblS_MAX>	m_strOption;
	std::bitset<10>	m_bitOBS;

	void	DefaultSetting(void);

public:
	CGRBLcOption();

	int			GetIntOpt(size_t);
	float		GetFloatOpt(size_t);
	std::string	GetStringOpt(size_t);
	bool		IsOBScheck(size_t);

	void		SetWPos(int nPos) {m_nWPos = nPos;}

	bool	Read(void);
	bool	Write(void);
	bool	Write_NoSetup(void);
};
