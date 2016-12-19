#pragma once

#include "module.h"
#include "data/story.hpp"

class Tale : public IEvent
{
public:
	Tale();
	~Tale();

public:
	// 接口
	void Run();

private:
	// 逻辑
	void _Next(std::string& msg); // 下一步

private:
	// 事件
	void _EventTest(const Event*); // 测试
	void _EventExit(const Event*); // 退出
	void _EventNew(const Event*); // 新游戏
	void _EventLoad(const Event*); // 读取
	void _EventSave(const Event*); // 保存

private:
	// 常量
	enum
	{
		//Next_
	};

private:
	// 外部引用
	ArchiveMgr& m_archiveMgr;
	StoryMgr& m_storyMgr;
private:
	// 内部参数
	bool m_run; // 正常运行中
	bool m_isBegin; // 是否开始游戏(新游戏/读取存档会开始)

private:
	// 数据
	SaveData m_save; // 存档数据
};
