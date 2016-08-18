/************************************************************************/
// 创建时间: 081123
// 最后确定: 081124

#ifndef _GameMap_h_
#define _GameMap_h_

#include "../../AnyPlay.h"
#include "../../Sprites/Sprite.h"

namespace AnyPlay
{
#define MB_EMPTY			0	// 空(在陆路模式能过)
#define MB_WATER			5	// 水(在水路模式能过)
#define MB_WALL				10	// 墙(永远无法通过)
#define MB_TRAP				15	// 陷阱(一旦经过就会关闭)
#define MB_ROCK				20	// 滚石(可成串推动,遇到混沌土则消失)
#define MB_ROCK_TAR			25	// 混沌土(不可通过)
#define MB_STEPON_TAR		30	// 踩踏门目标(只有被别的对象打开)
#define MB_END				35	// 终点(当两主角钻石总和为通过值则打开)

#define MB_RED_KEY			40	// 红水晶(经过一个门则消失,门被永久打开)
#define MB_BLUE_KEY			45	// 蓝水晶
#define MB_BLACK_KEY		50	// 黑水晶
#define MB_YELLOW_KEY		55	// 黄水晶
#define MB_PINK_KEY			60	// 粉水晶
#define MB_PASS				65	// 通行证(除非丢弃否则不消失,经过后关闭通行证门)
#define MB_MAIL				70	// 传送器(丢在地上再往里丢东西)
#define MB_GEM				75	// 钻石(每丢弃钻石检测一次终点打开/关闭)

#define MB_RED_SWITCH		80	// 红水晶门
#define MB_BLUE_SWITCH		85	// 蓝水晶门
#define MB_BLACK_SWITCH		90	// 黑水晶门
#define MB_YELLOW_SWITCH	95	// 黄水晶门
#define MB_PINK_SWITCH		100	// 粉水晶门
#define MB_STEPON_SWITCH	105	// 踩踏开关(当踩踏后打开目标门,离开时检测队友是否安全)
#define MB_PASS_SWITCH		110	// 通行证门
#define MB_MALE_SWITCH		115	// 男门
#define MB_FEMALE_SWITCH	120	// 女门
#define MB_WATER_SWITCH		125	// 水遁门
#define MB_CH_SWITCH		130	// 可改变门改变开关(踩踏后改变一次所有可变单向门)
#define MB_PORTAL			135	// 传送门(踩踏后传送到新坐标)

#define MB_UP_OW			140	// 单向门 上
#define MB_DOWN_OW			145	// 单向门 下
#define MB_LEFT_OW			150	// 单向门 左
#define MB_RIGHT_OW			155	// 单向门 右
#define MB_UP_CH			160	// 可变单向门 上
#define MB_DOWN_CH			165	// 可变单向门 下
#define MB_LEFT_CH			170	// 可变单向门 左
#define MB_RIGHT_CH			175	// 可变单向门 右

#define GAME_STEP_X			32.0f
#define GAME_STEP_Y			24.0f

	class GameInf;
	class GameMap;
	class MapBase;
	class Role;
	//////////////////////////////////////////////////////////////////////////
	enum DIRECT
	{
		DIR_UP = VK_UP,
		DIR_DOWN = VK_DOWN,
		DIR_LEFT = VK_LEFT,
		DIR_RIGHT = VK_RIGHT
	};


	//////////////////////////////////////////////////////////////////////////
	// 人物
#define ST_IDLE			0
#define ST_WALK_UP		1
#define ST_WALK_DOWN	2
#define ST_WALK_LEFT	3
#define ST_WALK_RIGHT	4

	enum Amphibious
	{
		Water,
		Land,
		Medium
	};

	class Role : public Sprite
	{
	public:
		friend class GameMap;
		friend class MapBase;
	public:
		Role();

		bool	RoleMove(DIRECT dir);
		void	Walk(DIRECT dir);
		void	FaceTo(DIRECT dir);

		void	StateIdle();
		bool	OnFrame(const EventArgs& args);
		void	UpdatePos();

		bool	Discard(ushort type);

	public:
		Amphibious	m_Amp;
		GameMap*	m_pMap;
		ushort		m_State;
		dword		m_X;
		dword		m_Y;
		DIRECT		m_Direct;

		ushort		m_RedKey;
		ushort		m_BlueKey;
		ushort		m_BlackKey;
		ushort		m_YellowKey;
		ushort		m_PinkKey;
		ushort		m_Mail;
		ushort		m_Gem;
		bool		m_bPass;
		bool		m_Passed;
	};


	//////////////////////////////////////////////////////////////////////////
	// 地图元素
	class MapBase : public Sprite
	{
	public:
		friend class GameMap;
		friend class Role;
	public:
		MapBase();
		ushort	GetType() { return m_Type; }

		void	Open();

		bool	CanLeave(Role* r, DIRECT dir);
		void	Leave(Role* r, DIRECT dir);
		bool	Push(Role* r, DIRECT dir);
		void	Stand(Role* r, DIRECT dir);
		void	UpdatePos();

		void	Walk(DIRECT dir);
		void	FaceTo(DIRECT dir);

		bool	OnFrame(const EventArgs& args);

	public:
		ushort		m_Type;
		ushort		m_State;
		DIRECT		m_Direct;
		dword		m_X;
		dword		m_Y;
		dword		m_TarX;
		dword		m_TarY;
		ushort		m_Lock;
		GameMap*	m_pMap;
		bool		m_bHitTar;
		MapBase*	m_TmpMB;
	};
	

	//////////////////////////////////////////////////////////////////////////
	// 游戏地图
	class GameMap
	{
	public:
		GameMap() : m_Width(0), m_Height(0), m_Hero(NULL), m_Heroine(NULL), m_TotalGems(0) {}
		~GameMap() { /*Clear(); */}

		void		CreateMap(dword width, dword height);
		bool		SetMapData(dword X, dword Y, MapBase* mb);
		MapBase*	GetMapData(dword X, dword Y);
		std::vector<MapBase*>	GetAll(ushort mbType);
		void		SetAll(MapBase* mb);
		bool		SwapData(dword sx, dword sy, dword dx, dword dy);
		void		SetEmpty(dword X, dword Y, dword W = 1, dword H = 1);
		void		SetWall(dword X, dword Y, dword W = 1, dword H = 1);
		void		SetWater(dword X, dword Y, dword W = 1, dword H = 1);

		void		Fill(dword X, dword Y, dword W, dword H, MapBase* mb);

		Role*		GetRole(dword X, dword Y);
		void		SetHero(Role* r) { m_Hero = r; r->m_pMap = this; r->UpdatePos(); }
		void		SetHeroine(Role* r) { m_Heroine = r; r->m_pMap = this; r->UpdatePos(); }

		bool		SaveMap(const String& filename);

	public:
		Role*		m_Hero;
		Role*		m_Heroine;
		ushort		m_TotalGems;
	private:
		MapBase*	m_MapData[65535];
		dword		m_Width;
		dword		m_Height;

	};

	bool	GetNextPos(dword* oldX, dword* oldY, DIRECT dir);
	DIRECT	InvDir(DIRECT dir);

	MapBase* AddWall(GameInf* gi, LPCTSTR name, dword X, dword Y);
	MapBase* AddPortal(GameInf* gi, LPCTSTR name, dword X, dword Y, dword tarX, dword tarY, ushort idx = 1);
	MapBase* AddTrap(GameInf* gi, LPCTSTR name, dword X, dword Y, ushort idx);
	MapBase* AddWater(GameInf* gi, LPCTSTR name, dword X, dword Y);
	MapBase* AddRock(GameInf* gi, LPCTSTR name, dword X, dword Y);
	MapBase* AddRockTar(GameInf* gi, LPCTSTR name, dword X, dword Y);
	MapBase* AddOW(GameInf* gi, LPCTSTR name, dword X, dword Y, DIRECT dir, bool ch);
	MapBase* AddThing(GameInf* gi, LPCTSTR name, dword X, dword Y, ushort type, LPCTSTR filename = NULL);
	MapBase* AddSwitch(GameInf* gi, LPCTSTR name, dword X, dword Y, ushort type, LPCTSTR filename = NULL, dword tarX = 0, dword tarY = 0);
}

#endif