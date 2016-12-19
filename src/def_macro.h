#pragma once

/************************************************************************/
/* NULL                                                                 */
/************************************************************************/
#ifndef NULL
#define NULL 0
#endif

/************************************************************************/
/* 单例                                                                  */
/************************************************************************/
#define SINGLETON_GETINSTANCE(T) \
public:\
	static T* getInstance()\
	{\
	static T* _pInstance = NULL; \
if (_pInstance == NULL)\
	_pInstance = new T; \
	return _pInstance; \
	}

#define SINGLETON(T) \
private:\
	T() {}\
	SINGLETON_GETINSTANCE(T)

/************************************************************************/
/* 指针校验                                                              */
/************************************************************************/
#define CONFIRM(p) {if(p==NULL){return;}}

/************************************************************************/
/* 数组参数获取                                                           */
/************************************************************************/
#define ARR_READ_BEGIN(ARR) int macro_readParamIdx = -1; int macro_readParamSize = ARR.size();
#define ARR_READ_I(ARR) ((macro_readParamSize > (++macro_readParamIdx)) ? (atoi(ARR[macro_readParamIdx].c_str())) : (0))
#define ARR_READ_S(ARR) ((macro_readParamSize > (++macro_readParamIdx)) ? (ARR[macro_readParamIdx]) : (""))
