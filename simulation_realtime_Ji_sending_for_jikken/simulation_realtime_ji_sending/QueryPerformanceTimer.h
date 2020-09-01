// Masahiro Furukawa
// Aug 24, 2020
//
// Interval Timer 
//
// reference ; https://misakichi-k.hatenablog.com/entry/2018/10/19/010134#WaitableTimer%E3%81%AB%E3%82%88%E3%82%8B%E5%9B%BA%E5%AE%9AFPS
//

#pragma once

#include <Windows.h>

#define ENABLE_ERROR_CORRECTION

class QueryPerformanceTimer {

private:
	unsigned int	fps_;
	bool			preframeIsDelay_;
	HANDLE			timer_;
	LONGLONG		freq_;
	LONGLONG		preframeTime_;
	LONGLONG		waitTime_;

public:
	QueryPerformanceTimer() {
		//default value
		fps_ = 60;
		timer_ = CreateWaitableTimer(NULL, FALSE, NULL);
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq_);
		preframeTime_ = getTime();
		waitTime_ = -msTo100Ns(1000) / fps_;
		SetWaitableTimer(timer_, (LARGE_INTEGER*)&waitTime_, 0, NULL, NULL, FALSE);
	}
	void setFps(unsigned int fps) {
		fps_ = fps;
	}
	LONGLONG getWaitTime() {
		return waitTime_;
	}
	LONGLONG getTime() {
		LONGLONG tm;
		QueryPerformanceCounter((LARGE_INTEGER*)&tm);
		return tm;
	}
	LONGLONG msTo100Ns(LONGLONG ms) {
		return usTo100Ns(ms * 1000);
	}
	LONGLONG usTo100Ns(LONGLONG us) {
		return us * 10;
	}
	void wait() {
		//timer object wait. one frame time each if to timeout.

#ifdef ENABLE_ERROR_CORRECTION
		auto waitRet = WaitForSingleObject(timer_, (1000 + fps_ - 1) / fps_);
#else
		auto waitRet = WaitForSingleObject(timer_, INFINITE);
#endif
		auto current = getTime();
#ifdef ENABLE_ERROR_CORRECTION
		auto sub = (current - preframeTime_) - freq_ / fps_;
		auto delay = waitRet == WAIT_TIMEOUT;
		if (delay == false && preframeIsDelay_ == false) {
			waitTime_ += sub;
		}
		preframeIsDelay_ = delay;
#endif
		SetWaitableTimer(timer_, (LARGE_INTEGER*)&waitTime_, 0, NULL, NULL, FALSE);
		preframeTime_ = getTime();
	}
};