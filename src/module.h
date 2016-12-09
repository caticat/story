#pragma once

#include <string>
#include <map>
#include <list>

#include "def_macro.h"
#include "jsoncpp/json/json.h"

/************************************************************************/
/* 事件                                                                  */
/************************************************************************/

/*
	功能
		有
			基础事件机制
			整理移除注册事件
		无
			校验事件重复调用
	用法
		构造函数注册
		析构函数取消注册(不取消会出问题)
*/

#define EVENT_REG(ID,FUN) EventMgr::getInstance()->Reg(ID, EventFunction((IEvent*)this, (event_fun_t)&FUN));
#define EVENT_UNREG() EventMgr::getInstance()->Unreg(this);
#define EVENT_PARAM(T,O,I) const T& O = *((T*)I);

struct Event // 基本事件
{
	Event() {};
};

struct EventTest : public Event // 测试事件
{
	std::string m_str1;
	std::string m_str2;

	EventTest(): Event(), m_str1(""), m_str2("") {}
};

class IEvent // 函数接口
{
};

typedef void(IEvent::*event_fun_t)(const Event*);

struct EventFunction // 事件调用函数
{
	IEvent* m_ptr;
	event_fun_t m_fun;

	EventFunction(IEvent* ptr, event_fun_t fun) : m_ptr(ptr), m_fun(fun) {}
	bool Empty() const { return ((m_ptr == NULL) || (m_fun == NULL)); }
};

class EventMgr
{
	SINGLETON(EventMgr);
public:
	enum
	{
		EVENT_TEST = 1, // 测试事件

		EVENT_MAX, // 最大值
	};

public:
	void Reg(int, const EventFunction&); // 注册
	void Unreg(IEvent* ptr); // 取消注册
	void Trigger(int, Event*); // 触发

private:
	std::map<int, std::list<EventFunction> > m_data; // 事件列表<事件id,注册函数列表>
};

/************************************************************************/
/* 数据存储                                                              */
/************************************************************************/

struct ISavePtrFun;

struct ISave // 存档 接口
{
public:
	int m_version; // 版本号

protected:
	std::map<int, ISavePtrFun> m_versionLoad; // 按版本号解析数据

public:
	ISave() : m_version(0) { m_versionLoad.clear(); }
	virtual void Reset() = 0;
	virtual std::string Get() const = 0;
	virtual void Set(std::string) = 0;
};

typedef void(ISave::*save_fun_t)(Json::Value&);

struct ISavePtrFun
{
	ISave* m_ptr;
	save_fun_t m_fun;

	ISavePtrFun(ISave* ptr, save_fun_t fun) : m_ptr(ptr), m_fun(fun) {}
	bool Empty() const { return ((m_ptr == NULL) || (m_fun == NULL)); }
};

struct SaveDataG : public ISave	// 通用数据
{
public:
	std::string m_test;

private:
	enum
	{
		ISave_V1 = 0, // 初始版本号

		ISave_VMax, // 最大版本号
	};

public:
	SaveDataG();
	void Reset();
	std::string Get() const;
	void Set(std::string);
protected:
	void SetV1(Json::Value&);
};

struct SaveDataP : public ISave	// 独立数据
{
public:
	std::string m_name;

private:
	enum
	{
		ISave_V1 = 0, // 初始版本号

		ISave_VMax, // 最大版本号
	};

public:
	SaveDataP();
	void Reset();
	std::string Get() const;
	void Set(std::string);
protected:
	void SetV1(Json::Value&);
};

struct SaveData
{
	SaveDataG m_gen;
	SaveDataP m_per;
};

class ArchiveMgr
{
private:
	ArchiveMgr();
	SINGLETON_GETINSTANCE(ArchiveMgr);
	void PathS(std::string path);
public:
	bool Save(int, const SaveData&);
	bool Load(int, SaveData&);

private:
	std::string _Name(int); // 获取存档名

public:
	enum
	{
		Archive_Id_Max = 99, // 存档最大数量
		Archive_Name_Length = 255, // 存档名长度
	};

public: // 存档
	std::string m_name;

private: // 内存
	std::string m_path;
};
