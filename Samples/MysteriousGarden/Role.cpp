/************************************************************************/
// 创建时间: 081123
// 最后确定: 081124

#include "Game.h"
#include "GameInf.h"

namespace AnyPlay
{
	Role::Role() : m_State(ST_IDLE), m_X(0), m_Y(0), m_pMap(NULL), m_Direct(DIR_DOWN), m_Amp(Land), m_Passed(false)
	{
		m_RedKey = 0;
		m_BlueKey = 0;
		m_BlackKey = 0;
		m_YellowKey = 0;
		m_PinkKey = 0;
		m_Mail = 0;
		m_Gem = 0;
		m_bPass = false;

		SubscribeEvent(FrmEvent, SubscriberSlot(&Role::OnFrame, this));
	}

	void Role::UpdatePos()
	{
		SetPosition(Point(m_X * GAME_STEP_X, m_Y * GAME_STEP_Y));
		Sort_Y();
	}

	bool Role::RoleMove(DIRECT dir)
	{
		if(!m_pMap) return false;
		if(m_State == ST_IDLE)
		{
			dword newX = m_X;
			dword newY = m_Y;
			GetNextPos(&newX, &newY, dir);

			Role* role = m_pMap->GetRole(newX, newY);
			MapBase* mbNew = m_pMap->GetMapData(newX, newY);
			MapBase* mbOld = m_pMap->GetMapData(m_X, m_Y);

			if(!role)
			{
				if(mbNew && mbOld)
				{
					if(mbOld->CanLeave(this, dir))
					{
						if(mbNew->Push(this, dir))
						{
							mbOld->Leave(this, dir);
							Walk(dir);
							GameInf::GetSingleton().UpdateThings();
							return true;
						}
					}
				}
			}
			FaceTo(dir);
		}
		return false;
	}

	void Role::Walk(DIRECT dir)
	{
		m_Direct = dir;

		Play();

		if(dir == DIR_UP)
		{
			m_State = ST_WALK_UP;
			Goto(_T("上行"));
			m_Y--;
		}
		else if(dir == DIR_DOWN)
		{
			m_State = ST_WALK_DOWN;
			Goto(_T("下行"));
			m_Y++;
		}
		else if(dir == DIR_LEFT)
		{
			m_State = ST_WALK_LEFT;
			Goto(_T("左行"));
			m_X--;
		}
		else if(dir == DIR_RIGHT)
		{
			m_State = ST_WALK_RIGHT;
			Goto(_T("右行"));
			m_X++;
		}
	}

	void Role::FaceTo(DIRECT dir)
	{
		m_Direct = dir;
		if(dir == DIR_UP)
		{
			Goto(_T("上"));
		}
		else if(dir == DIR_DOWN)
		{
			Goto(_T("下"));
		}
		else if(dir == DIR_LEFT)
		{
			Goto(_T("左"));
		}
		else if(dir == DIR_RIGHT)
		{
			Goto(_T("右"));
		}
	}

	void Role::StateIdle()
	{
		if(m_State != ST_IDLE)
		{
			m_State = ST_IDLE;
			MapBase* pMB = m_pMap->GetMapData(m_X, m_Y);
			if(pMB) pMB->Stand(this, m_Direct);
			GameInf::GetSingleton().UpdateThings();
		}
	}

	bool Role::OnFrame(const EventArgs& args)
	{
		if(m_State == ST_WALK_UP)
		{
			if(GetCurrentFrmPos() == 51)
			{
				StateIdle();
			}
			else
			{
				m_Position.y -= GAME_STEP_Y / 16.0f;
				Sort_Y();
			}
		}
		else if(m_State == ST_WALK_DOWN)
		{
			if(GetCurrentFrmPos() == 0)
			{
				StateIdle();
			}
			else
			{
				m_Position.y += GAME_STEP_Y / 16.0f;
				Sort_Y();
			}
		}
		else if(m_State == ST_WALK_LEFT)
		{
			if(GetCurrentFrmPos() == 17)
			{
				StateIdle();
			}
			else
			{
				m_Position.x -= GAME_STEP_X / 16.0f;
			}
		}
		else if(m_State == ST_WALK_RIGHT)
		{
			if(GetCurrentFrmPos() == 34)
			{
				StateIdle();
			}
			else
			{
				m_Position.x += GAME_STEP_X / 16.0f;
			}
		}
		return true;
	}


	bool Role::Discard(ushort type)
	{
		dword newX = m_X;
		dword newY = m_Y;
		GetNextPos(&newX, &newY, m_Direct);

		Role* newRole = m_pMap->GetRole(newX, newY);
		MapBase* mbNew = m_pMap->GetMapData(newX, newY);

		bool bMail = false;
		if(mbNew)
		{
			if(mbNew->m_Type == MB_MAIL) bMail = true;
		}

		if(newRole || bMail)	// 人或传送器
		{
			if(!newRole)
			{
				newRole = ((this == m_pMap->m_Hero) ? m_pMap->m_Heroine : m_pMap->m_Hero);
			}
			if(type == MB_PASS)
			{
				if(newRole->m_bPass) return true;
				else
				{
					if(!m_bPass) return false;
					m_bPass = false;
					newRole->m_bPass = true;
				}
			}
			else
			{
				if(type == MB_BLUE_KEY)
				{
					if(!m_BlueKey) return false;
					m_BlueKey--;
					newRole->m_BlueKey++;
				}
				else if(type == MB_RED_KEY)
				{
					if(!m_RedKey) return false;
					m_RedKey--;
					newRole->m_RedKey++;
				}
				else if(type == MB_BLACK_KEY)
				{
					if(!m_BlackKey) return false;
					m_BlackKey--;
					newRole->m_BlackKey++;
				}
				else if(type == MB_YELLOW_KEY)
				{
					if(!m_YellowKey) return false;
					m_YellowKey--;
					newRole->m_YellowKey++;
				}
				else if(type == MB_PINK_KEY)
				{
					if(!m_PinkKey) return false;
					m_PinkKey--;
					newRole->m_PinkKey++;
				}
				else if(type == MB_MAIL)
				{
					if(!m_Mail) return false;
					m_Mail--;
					newRole->m_Mail++;
				}
			}
			if(bMail)
			{
				mbNew->SetDetachFlag(APNODE_REMOVE);
				m_pMap->SetEmpty(newX, newY);
			}
			GameInf::GetSingleton().UpdateThings();
			return true;
		}
		else if(mbNew)
		{
			GameInf* gi = GameInf::GetSingletonPtr();
			if(mbNew->m_Type == MB_EMPTY)
			{
				if(type == MB_BLUE_KEY)
				{
					if(!m_BlueKey) return false;
					m_BlueKey--;
				}
				else if(type == MB_RED_KEY)
				{
					if(!m_RedKey) return false;
					m_RedKey--;
				}
				else if(type == MB_BLACK_KEY)
				{
					if(!m_BlackKey) return false;
					m_BlackKey--;
				}
				else if(type == MB_YELLOW_KEY)
				{
					if(!m_YellowKey) return false;
					m_YellowKey--;
				}
				else if(type == MB_PINK_KEY)
				{
					if(!m_PinkKey) return false;
					m_PinkKey--;
				}
				else if(type == MB_MAIL)
				{
					if(!m_Mail) return false;
					m_Mail--;
				}
				else if(type == MB_PASS)
				{
					if(!m_bPass) return false;
					m_bPass = false;
				}
				AddThing(gi, 0, newX, newY, type);
			}
			/*else if(mbNew->m_Type == MB_MAIL)
			{
	
			}// 没代码*/

			GameInf::GetSingleton().UpdateThings();
			return true;
		}

		return false;
	}

}