/************************************************************************/
// ����ʱ��: 081123
// ���ȷ��: 081124

#ifndef _GameMap_h_
#define _GameMap_h_

#include "../../AnyPlay.h"
#include "../../Sprites/Sprite.h"

namespace AnyPlay
{
#define MB_EMPTY			0	// ��(��½·ģʽ�ܹ�)
#define MB_WATER			5	// ˮ(��ˮ·ģʽ�ܹ�)
#define MB_WALL				10	// ǽ(��Զ�޷�ͨ��)
#define MB_TRAP				15	// ����(һ�������ͻ�ر�)
#define MB_ROCK				20	// ��ʯ(�ɳɴ��ƶ�,��������������ʧ)
#define MB_ROCK_TAR			25	// ������(����ͨ��)
#define MB_STEPON_TAR		30	// ��̤��Ŀ��(ֻ�б���Ķ����)
#define MB_END				35	// �յ�(����������ʯ�ܺ�Ϊͨ��ֵ���)

#define MB_RED_KEY			40	// ��ˮ��(����һ��������ʧ,�ű����ô�)
#define MB_BLUE_KEY			45	// ��ˮ��
#define MB_BLACK_KEY		50	// ��ˮ��
#define MB_YELLOW_KEY		55	// ��ˮ��
#define MB_PINK_KEY			60	// ��ˮ��
#define MB_PASS				65	// ͨ��֤(���Ƕ���������ʧ,������ر�ͨ��֤��)
#define MB_MAIL				70	// ������(���ڵ��������ﶪ����)
#define MB_GEM				75	// ��ʯ(ÿ������ʯ���һ���յ��/�ر�)

#define MB_RED_SWITCH		80	// ��ˮ����
#define MB_BLUE_SWITCH		85	// ��ˮ����
#define MB_BLACK_SWITCH		90	// ��ˮ����
#define MB_YELLOW_SWITCH	95	// ��ˮ����
#define MB_PINK_SWITCH		100	// ��ˮ����
#define MB_STEPON_SWITCH	105	// ��̤����(����̤���Ŀ����,�뿪ʱ�������Ƿ�ȫ)
#define MB_PASS_SWITCH		110	// ͨ��֤��
#define MB_MALE_SWITCH		115	// ����
#define MB_FEMALE_SWITCH	120	// Ů��
#define MB_WATER_SWITCH		125	// ˮ����
#define MB_CH_SWITCH		130	// �ɸı��Ÿı俪��(��̤��ı�һ�����пɱ䵥����)
#define MB_PORTAL			135	// ������(��̤���͵�������)

#define MB_UP_OW			140	// ������ ��
#define MB_DOWN_OW			145	// ������ ��
#define MB_LEFT_OW			150	// ������ ��
#define MB_RIGHT_OW			155	// ������ ��
#define MB_UP_CH			160	// �ɱ䵥���� ��
#define MB_DOWN_CH			165	// �ɱ䵥���� ��
#define MB_LEFT_CH			170	// �ɱ䵥���� ��
#define MB_RIGHT_CH			175	// �ɱ䵥���� ��

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
	// ����
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
	// ��ͼԪ��
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
	// ��Ϸ��ͼ
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