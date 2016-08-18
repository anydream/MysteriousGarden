/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123

#include "Game.h"
#include "../../FrameWork/NodeManager.h"

namespace AnyPlay
{
	/*void GameMap::Clear()
	{
		dword total = m_Width * m_Height;
		for(dword i = 0; i < total; i++)
		{
			SAFE_DELETE(m_MapData[i]);
		}
	}*/

	void GameMap::CreateMap(dword width, dword height)
	{
		m_Width = width;
		m_Height = height;
		for(dword i = 0; i < m_Height; i++)		//Y
		{
			for(dword j = 0; j < m_Width; j++)	//X
			{
				m_MapData[i * m_Width + j] = NULL;
			}
		}
	}

	bool GameMap::SetMapData(dword X, dword Y, MapBase* mb)
	{
		if(X < m_Width && Y < m_Height)
		{
			m_MapData[Y * m_Width + X] = mb;
			mb->m_X = X;
			mb->m_Y = Y;
			mb->m_pMap = this;
			return true;
		}
		return false;
	}

	MapBase* GameMap::GetMapData(dword X, dword Y)
	{
		if(X < m_Width && Y < m_Height)
		{
			return m_MapData[Y * m_Width + X];
		}
		return NULL;
	}

	std::vector<MapBase*> GameMap::GetAll(ushort mbType)
	{
		std::vector<MapBase*> lst;
		dword total = m_Width * m_Height;
		for(dword i = 0; i < total; i++)
		{
			if(m_MapData[i])
			{
				//X Y呢？
				if(m_MapData[i]->m_Type == mbType) lst.push_back(m_MapData[i]);
			}
		}
		return lst;
	}

	void GameMap::SetAll(MapBase* mb)
	{
		dword total = m_Width * m_Height;
		for(dword i = 0; i < total; i++)
		{
			m_MapData[i] = mb;
			mb->m_pMap = this;
		}
	}

	Role* GameMap::GetRole(dword X, dword Y)
	{
		if(X < m_Width && Y < m_Height)
		{
			if(m_Hero)
			{
				if(m_Hero->m_X == X && m_Hero->m_Y == Y) return m_Hero;
			}
			if(m_Heroine)
			{
				if(m_Heroine->m_X == X && m_Heroine->m_Y == Y) return m_Heroine;
			}
		}
		return NULL;
	}

	bool GameMap::SwapData(dword sx, dword sy, dword dx, dword dy)
	{
		if(sx < m_Width && sy < m_Height && dx < m_Width && dy < m_Height)
		{
			MapBase* mbTmp = m_MapData[sy * m_Width + sx];
			m_MapData[sy * m_Width + sx] = m_MapData[dy * m_Width + dx];
			m_MapData[dy * m_Width + dx] = mbTmp;
			return true;
		}
		return false;
	}

	void GameMap::Fill(dword X, dword Y, dword W, dword H, MapBase* mb)
	{
		for(int i = Y; i < Y + H; i++)
		{
			for(int j = X; j < X + W; j++)
			{
				SetMapData(j, i, mb);
			}
		}
	}

	void GameMap::SetEmpty(dword X, dword Y, dword W, dword H)
	{
		MapBase* mbEmpty = (MapBase*)NodeManager::GetSingleton().Get(_T("空"));
		Fill(X, Y, W, H, mbEmpty);
	}


	void GameMap::SetWall(dword X, dword Y, dword W, dword H)
	{
		MapBase* mbWall = (MapBase*)NodeManager::GetSingleton().Get(_T("墙"));
		Fill(X, Y, W, H, mbWall);
	}

	void GameMap::SetWater(dword X, dword Y, dword W, dword H)
	{
		MapBase* mbWater = (MapBase*)NodeManager::GetSingleton().Get(_T("水"));
		Fill(X, Y, W, H, mbWater);
	}


	bool GameMap::SaveMap(const String& filename)
	{
		FILE* fp = _tfopen(filename(), _T("w"));
		if(fp)
		{
			for(dword i = 0; i < m_Height; i++)		//Y
			{
				for(dword j = 0; j < m_Width; j++)	//X
				{
					MapBase* mb = m_MapData[i * m_Width + j];

					switch(mb->m_Type)
					{
					case MB_WALL:
						_ftprintf(fp, _T("■")); break;
					case MB_EMPTY:
						_ftprintf(fp, _T("  ")); break;
					case MB_WATER:
						_ftprintf(fp, _T("▓")); break;
					default:
						_ftprintf(fp, _T("%02X"), mb->m_Type);
					}
				}
				_ftprintf(fp, _T("\n"));
			}
			fclose(fp);

			return true;
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	bool GetNextPos(dword* oldX, dword* oldY, DIRECT dir)
	{
		if(oldX && oldY)
		{
			if(dir == DIR_UP)
				(*oldY)--;
			else if(dir == DIR_DOWN)
				(*oldY)++;
			else if(dir == DIR_LEFT)
				(*oldX)--;
			else if(dir == DIR_RIGHT)
				(*oldX)++;
			return true;
		}
		return false;
	}

	DIRECT InvDir(DIRECT dir)
	{
		if(dir == DIR_UP) return DIR_DOWN;
		if(dir == DIR_DOWN) return DIR_UP;
		if(dir == DIR_LEFT) return DIR_RIGHT;
		if(dir == DIR_RIGHT) return DIR_LEFT;
		return DIR_UP;
	}
}