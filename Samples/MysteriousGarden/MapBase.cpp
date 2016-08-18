/************************************************************************/
// ����ʱ��: 081123
// ���ȷ��: 081124

#include "Game.h"
#include "GameInf.h"

namespace AnyPlay
{
	MapBase::MapBase() : m_Type(MB_EMPTY), m_pMap(NULL), m_State(ST_IDLE), m_Direct(DIR_UP),
		m_X(0), m_Y(0), m_TarX(0), m_TarY(0), m_Lock(0), m_bHitTar(false), m_TmpMB(NULL)
	{
		SubscribeEvent(FrmEvent, SubscriberSlot(&MapBase::OnFrame, this));
	}

	void MapBase::Open()
	{
		m_Lock--;
		if(m_Type == MB_ROCK_TAR)
		{
			Goto(_T("����"));
			Play();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ���»�ͼ����
	void MapBase::UpdatePos()
	{
		if( m_Type == MB_TRAP ||
			m_Type == MB_PORTAL)
			SetPosition(Point(m_X * GAME_STEP_X - 1.0f, (m_Y - 1) * GAME_STEP_Y));

		else if(m_Type == MB_RED_SWITCH ||
				m_Type == MB_BLUE_SWITCH ||
				m_Type == MB_BLACK_SWITCH ||
				m_Type == MB_YELLOW_SWITCH ||
				m_Type == MB_PINK_SWITCH ||
				m_Type == MB_STEPON_SWITCH ||
				m_Type == MB_STEPON_TAR ||
				m_Type == MB_MALE_SWITCH ||
				m_Type == MB_FEMALE_SWITCH ||
				m_Type == MB_WATER_SWITCH)
				SetPosition(Point(m_X * GAME_STEP_X, (m_Y - 1) * GAME_STEP_Y));

		else if(m_Type == MB_UP_CH ||
				m_Type == MB_DOWN_CH ||
				m_Type == MB_LEFT_CH ||
				m_Type == MB_RIGHT_CH ||
				m_Type == MB_UP_OW ||
				m_Type == MB_DOWN_OW ||
				m_Type == MB_LEFT_OW ||
				m_Type == MB_RIGHT_OW)
			SetPosition(Point(m_X * GAME_STEP_X, (m_Y - 1) * GAME_STEP_Y + 10.f));

		else if(m_Type == MB_RED_KEY ||
				m_Type == MB_BLUE_KEY ||
				m_Type == MB_BLACK_KEY ||
				m_Type == MB_YELLOW_KEY ||
				m_Type == MB_PINK_KEY ||
				m_Type == MB_MAIL ||
				m_Type == MB_GEM ||
				m_Type == MB_PASS)
			SetPosition(Point(m_X * GAME_STEP_X, m_Y * GAME_STEP_Y - 7.0f));
		
		else if(m_Type == MB_END)
			SetPosition(Point(m_X * GAME_STEP_X, m_Y * GAME_STEP_Y) - 9.0f);

		else if(m_Type == MB_WALL)
			SetPosition(Point(m_X * GAME_STEP_X, m_Y * GAME_STEP_Y + 2.0f));

		else SetPosition(Point(m_X * GAME_STEP_X, m_Y * GAME_STEP_Y));

		Sort_Y();
	}

	//////////////////////////////////////////////////////////////////////////
	// �����뿪��
	bool MapBase::CanLeave(Role* r, DIRECT dir)
	{
		/*if(m_Type == MB_END) return false;
		else */if(m_Type == MB_STEPON_SWITCH)
		{
			if(m_pMap->GetRole(m_TarX, m_TarY)) return false;	// �������Ƕ���
			else return true;
		}
		else if(m_Type == MB_UP_OW || m_Type == MB_UP_CH)
		{
			if(dir == DIR_UP) return true;
			else return false;		// ֻ�ܳ�ָ����������
		}
		else if(m_Type == MB_DOWN_OW || m_Type == MB_DOWN_CH)
		{
			if(dir == DIR_DOWN) return true;
			else return false;		// ֻ�ܳ�ָ����������
		}
		else if(m_Type == MB_LEFT_OW || m_Type == MB_LEFT_CH)
		{
			if(dir == DIR_LEFT) return true;
			else return false;		// ֻ�ܳ�ָ����������
		}
		else if(m_Type == MB_RIGHT_OW || m_Type == MB_RIGHT_CH)
		{
			if(dir == DIR_RIGHT) return true;
			else return false;		// ֻ�ܳ�ָ����������
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// �뿪
	void MapBase::Leave(Role* r, DIRECT dir)
	{
		if(m_Type == MB_TRAP)
		{
			m_Lock = 1;
			Goto(_T("�ر�"));
			Play();
		}
		else if(m_Type == MB_STEPON_SWITCH)
		{
			Goto(_T("�ر�"));
			Play();
			if(MapBase* mbTar = m_pMap->GetMapData(m_TarX, m_TarY))
			{
				mbTar->m_Lock = 1;
				mbTar->Goto(_T("�ر�"));
				mbTar->Play();
			}
		}
		else if(m_Type == MB_PASS_SWITCH ||
				m_Type == MB_MALE_SWITCH ||
				m_Type == MB_FEMALE_SWITCH ||
				m_Type == MB_WATER_SWITCH/*||
				m_Type == MB_CH_SWITCH*/)
		{
			Goto(_T("�ر�"));
			Play();
		}

	}

	//////////////////////////////////////////////////////////////////////////
	// ����̤
	void MapBase::Stand(Role* r, DIRECT dir)
	{
		if(m_Type == MB_WATER) r->m_Amp = Water;
		else if(m_Type == MB_WATER_SWITCH) r->m_Amp = Medium;	// ����ˮ��ģʽ
		else
		{
			if(r) r->m_Amp = Land;
			if(m_Type == MB_WALL) {}		// ����
			else if(m_Type == MB_TRAP) {}	// С��
			else if(m_Type == MB_END)
			{
				//if(!m_Lock) {}	// ������ʧr
				ushort numGems = m_pMap->m_Hero->m_Gem + m_pMap->m_Heroine->m_Gem;
				if(m_pMap->m_TotalGems == numGems)	// ��������е���ʯ
				{
					r->m_Passed = true;
					r->SetVisible(false);
					r->m_X = r->m_Y = -1;
					((GameInf*)GameInf::GetSingletonPtr())->ChangeCtrlRole();
				}
			}
			else if(m_Type == MB_PORTAL)
			{
				if(!m_pMap->GetRole(m_TarX, m_TarY))
				{
					// ����r
					r->m_X = m_TarX;
					r->m_Y = m_TarY;
					r->UpdatePos();
				}
			}
			else if(m_Type == MB_UP_OW || m_Type == MB_UP_CH) r->RoleMove(DIR_UP);
			else if(m_Type == MB_DOWN_OW || m_Type == MB_DOWN_CH) r->RoleMove(DIR_DOWN);
			else if(m_Type == MB_LEFT_OW || m_Type == MB_LEFT_CH) r->RoleMove(DIR_LEFT);
			else if(m_Type == MB_RIGHT_OW || m_Type == MB_RIGHT_CH) r->RoleMove(DIR_RIGHT);
			else if(m_Type == MB_ROCK)
			{
				m_State = ST_IDLE;
				if(m_bHitTar)
				{
					m_bHitTar = false;
					m_TmpMB->Goto(_T("����"));
					m_TmpMB->Play();
					m_pMap->SetEmpty(m_X, m_Y);
					SetDetachFlag(APNODE_REMOVE);
				}
			}
			else if(m_Type == MB_RED_KEY ||
					m_Type == MB_BLUE_KEY ||
					m_Type == MB_BLACK_KEY ||
					m_Type == MB_YELLOW_KEY ||
					m_Type == MB_PINK_KEY ||
					m_Type == MB_MAIL ||
					m_Type == MB_GEM ||
					m_Type == MB_PASS)
			{
				SetDetachFlag(APNODE_REMOVE);
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// �����ƶ���Ϣ
	bool MapBase::Push(Role* r, DIRECT dir)
	{
		bool bCanPush = true;
		if(r)
		{
			if(r->m_Amp == Water) bCanPush = false;
		}

		if(bCanPush)
		{
			if(m_Type == MB_EMPTY)
			{
				if(r)
				{
					if(r->m_Amp == Medium || r->m_Amp == Land)
					{
						//r->m_Amp = Land;
						return true;
					}
					else return false;
				}
				else return true;
			}
			else if(m_Type == MB_WATER)
			{
				if(r)
				{
					if(r->m_Amp == Medium/*|| r->m_Amp == Water*/)
					{
						//r->m_Amp = Water;
						return true;
					}
				}
				return false;
			}
			else if(m_Type == MB_WALL || m_Type == MB_ROCK_TAR) return false;
			else if(m_Type == MB_TRAP || m_Type == MB_STEPON_TAR)
			{
				if(m_Lock) return false;
				else return true;
			}
			else if(m_Type == MB_ROCK)
			{
				if(m_State == ST_IDLE)
				{
					dword oldX = m_X;
					dword oldY = m_Y;
					dword newX = m_X;
					dword newY = m_Y;
					GetNextPos(&newX, &newY, dir);

					if(!m_pMap->GetRole(newX, newY))
					{
						MapBase* mbNew = m_pMap->GetMapData(newX, newY);
						if(mbNew)
						{
							bool bPushed = true;
							if(mbNew->m_Type == MB_EMPTY) {}//m_pMap->SwapData(newX, newY, m_X, m_Y);
							else if(mbNew->m_Type == MB_ROCK) bPushed = mbNew->Push(NULL, dir);
							else if(mbNew->m_Type == MB_ROCK_TAR) m_bHitTar = true;
							else return false;

							if(bPushed)
							{
								Walk(dir);
								m_pMap->SetEmpty(oldX, oldY);
								return true;
							}
						}
					}

					FaceTo(dir);
				}
				return false;
			}
			else if(m_Type == MB_STEPON_SWITCH)
			{
				Goto(_T("����"));
				Play();
				if(MapBase* mbTar = m_pMap->GetMapData(m_TarX, m_TarY))
				{
					mbTar->m_Lock = 0;
					mbTar->Goto(_T("����"));
					mbTar->Play();
				}
				return true;
			}
			else if(m_Type == MB_RED_KEY ||
				m_Type == MB_BLUE_KEY ||
				m_Type == MB_BLACK_KEY ||
				m_Type == MB_YELLOW_KEY ||
				m_Type == MB_PINK_KEY ||
				m_Type == MB_MAIL ||
				m_Type == MB_GEM ||
				m_Type == MB_PASS)
			{
				if(m_Type == MB_RED_KEY) r->m_RedKey++;
				else if(m_Type == MB_BLUE_KEY) r->m_BlueKey++;
				else if(m_Type == MB_BLACK_KEY) r->m_BlackKey++;
				else if(m_Type == MB_YELLOW_KEY) r->m_YellowKey++;
				else if(m_Type == MB_PINK_KEY) r->m_PinkKey++;
				else if(m_Type == MB_MAIL) r->m_Mail++;
				else if(m_Type == MB_GEM) r->m_Gem++;//����㹻��ʯ�Ϳ����յ�
				else if(m_Type == MB_PASS)
				{
					if(r->m_bPass) return true;
					else r->m_bPass = true;
				}
				Goto(_T("��ʧ"));
				Play();
				m_pMap->SetEmpty(m_X, m_Y);
			}
			else if(m_Type == MB_RED_SWITCH ||
				m_Type == MB_BLUE_SWITCH ||
				m_Type == MB_BLACK_SWITCH ||
				m_Type == MB_YELLOW_SWITCH ||
				m_Type == MB_PINK_SWITCH)
			{
				bool bOpen = false;
				if(m_Type == MB_RED_SWITCH)
				{
					if(r->m_RedKey)
					{
						r->m_RedKey--;
						bOpen = true;
					}
				}
				else if(m_Type == MB_BLUE_SWITCH)
				{
					if(r->m_BlueKey)
					{
						r->m_BlueKey--;
						bOpen = true;
					}
				}
				else if(m_Type == MB_BLACK_SWITCH)
				{
					if(r->m_BlackKey)
					{
						r->m_BlackKey--;
						bOpen = true;
					}
				}
				else if(m_Type == MB_YELLOW_SWITCH)
				{
					if(r->m_YellowKey)
					{
						r->m_YellowKey--;
						bOpen = true;
					}
				}
				else if(m_Type == MB_PINK_SWITCH)
				{
					if(r->m_PinkKey)
					{
						r->m_PinkKey--;
						bOpen = true;
					}
				}
				if(bOpen)
				{
					Goto(_T("����"));
					Play();
					m_pMap->SetEmpty(m_X, m_Y);
					return true;
				}
				return false;
			}
			else if(m_Type == MB_PASS_SWITCH)
			{
				if(!r->m_bPass) return false;
				Goto(_T("����"));
				Play();
			}
			else if(m_Type == MB_CH_SWITCH)	// �л���
			{
				if(GetCurrentFrmPos() == 0) SetNextFrmPos(1);
				else SetNextFrmPos(0);
				Play();
				std::vector<MapBase*> upch, downch, leftch, rightch;
				upch = m_pMap->GetAll(MB_UP_CH);
				downch = m_pMap->GetAll(MB_DOWN_CH);
				leftch = m_pMap->GetAll(MB_LEFT_CH);
				rightch = m_pMap->GetAll(MB_RIGHT_CH);

				std::vector<MapBase*>::iterator i;
				for(i = upch.begin(); i != upch.end(); i++)
				{
					(*i)->m_Type = MB_DOWN_CH;
					(*i)->Goto(_T("����"));
				}
				for(i = downch.begin(); i != downch.end(); i++)
				{
					(*i)->m_Type = MB_UP_CH;
					(*i)->Goto(_T("����"));
				}
				for(i = leftch.begin(); i != leftch.end(); i++)
				{
					(*i)->m_Type = MB_RIGHT_CH;
					(*i)->Goto(_T("����"));
				}
				for(i = rightch.begin(); i != rightch.end(); i++)
				{
					(*i)->m_Type = MB_LEFT_CH;
					(*i)->Goto(_T("����"));
				}
			}
			else if(m_Type == MB_MALE_SWITCH)
			{
				if(r == m_pMap->m_Hero)
				{
					Goto(_T("����"));
					Play();
					return true;
				}
				return false;
			}
			else if(m_Type == MB_FEMALE_SWITCH)
			{
				if(r == m_pMap->m_Heroine)
				{
					Goto(_T("����"));
					Play();
					return true;
				}
				return false;
			}
			else if(m_Type == MB_WATER_SWITCH)
			{
				Goto(_T("����"));
				Play();
			}
			
			return true;
		}
		if(m_Type == MB_WATER_SWITCH)
		{
			Goto(_T("����"));
			Play();
			return true;
		}
		else if(m_Type == MB_WATER) return true;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ��ʯ �ƶ�����
	void MapBase::Walk(DIRECT dir)
	{
		m_Direct = dir;

		Play();

		if(dir == DIR_UP)
		{
			m_State = ST_WALK_UP;
			Goto(_T("����"));
			m_Y--;
		}
		else if(dir == DIR_DOWN)
		{
			m_State = ST_WALK_DOWN;
			Goto(_T("����"));
			m_Y++;
		}
		else if(dir == DIR_LEFT)
		{
			m_State = ST_WALK_LEFT;
			Goto(_T("����"));
			m_X--;
		}
		else if(dir == DIR_RIGHT)
		{
			m_State = ST_WALK_RIGHT;
			Goto(_T("����"));
			m_X++;
		}
		m_TmpMB = m_pMap->GetMapData(m_X, m_Y);
		m_pMap->SetMapData(m_X, m_Y, this);
	}

	//////////////////////////////////////////////////////////////////////////
	// ��ʯ �޷��ƶ��ͳ���
	void MapBase::FaceTo(DIRECT dir)
	{
		m_Direct = dir;
		if(dir == DIR_UP)
		{
			Goto(_T("��"));
		}
		else if(dir == DIR_DOWN)
		{
			Goto(_T("��"));
		}
		else if(dir == DIR_LEFT)
		{
			Goto(_T("��"));
		}
		else if(dir == DIR_RIGHT)
		{
			Goto(_T("��"));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ֡��Ӧ
	bool MapBase::OnFrame(const EventArgs& args)
	{
		if(m_Type == MB_ROCK)
		{
			if(m_State == ST_WALK_UP)
			{
				if(GetCurrentFrmPos() == 51)
				{
					Stand(NULL, m_Direct);
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
					Stand(NULL, m_Direct);
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
					Stand(NULL, m_Direct);
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
					Stand(NULL, m_Direct);
				}
				else
				{
					m_Position.x += GAME_STEP_X / 16.0f;
				}
			}
		}
		else if(m_Type == MB_ROCK_TAR)
		{
			if(GetCurrentFrmPos() == 6)
				SetDetachFlag(APNODE_REMOVE);
		}
		return true;
	}


}