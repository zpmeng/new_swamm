#ifndef __REAL_TIME_CLOCK_H__
#define __REAL_TIME_CLOCK_H__

#include "Locker.h"
#include "TimeoutThread.h"

class CRealTimeClock : public CTimeoutThread
{
public:
	CRealTimeClock();
	virtual ~CRealTimeClock();

public:
	BOOL	IsInstalled() const;
	BOOL	IsAutoSync() const;
	BOOL	IsNtpSync()	const;
	BOOL	IsSuccess() const;
	BOOL	SetTime(TIMESTAMP *pTime);
	BOOL	UpdateTime(char *pszTime);
#if defined(__TI_AM335X__)
	int		GetTemperature();
#endif
// Operations
public:
    BOOL    Initialize();
    void    Destroy();

	BOOL	Sync();
    BOOL    GetUtcTimestamp(TIMESTAMP *localTime, TIMESTAMP *utcTime, int gap); 

protected:
	BOOL	DetectRealTimeClock();
#if 0
    // Issue #1264 : 더이상 Mobile module에서 시간을 구하지 않는다.
	BOOL	GetTimeFromMobile();
#endif
	BOOL	GetTimeFromNetwork();
	BOOL	TimeSync();
	BOOL	TimesyncFromNetworkOrMobile();

protected:
	BOOL	OnActiveThread();
	BOOL	OnTimeoutThread();

private:
	CLocker		m_Locker;
	BOOL		m_bInstalled;
	BOOL		m_bSuccess;
	UINT		m_nLastSyncTime;
	char		m_szRtcNameSpace[64];
	BOOL		m_bFirst;
	BOOL		m_bNtpSync;
};

extern CRealTimeClock	*m_pRealTimeClock;

#endif	// __REAL_TIME_CLOCK_H__
