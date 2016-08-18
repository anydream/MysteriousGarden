/************************************************************************/
// ����ʱ��: 081029
// ���ȷ��: 081120

#include "Sprite.h"
#include "../RenderSystem/RenderSystem.h"

namespace AnyPlay
{
	bool Layer::DrawLayer(const Point& dstOff, const Point& srcOff, const Color& color)
	{
		if(m_pTexture && color.m_Alpha > 0.003f)
		{
			Color dstColor[4] =
			{
				m_Color[0] * color,
				m_Color[1] * color,
				m_Color[2] * color,
				m_Color[3] * color
			};
			return m_pTexture->DrawTexture(&(m_DestQuad + dstOff), &(m_SrcQuad + srcOff), dstColor);
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	void Sprite::DrawSprite(const Point& dstPos, const Point& srcPos, const Color& color)
	{
		if(GetVisible())
		{
			Color dstColor = m_Color * color;

			if(IN_RANGE(m_CurrentFrame, m_FrameList))
			{
				Color curColor = dstColor;
				if(m_bSmoothMode)
				{
					float weight = GetWeight();
					curColor.m_Alpha = curColor.m_Alpha * (weight + 1.0f) * weight;
					if(curColor.m_Alpha > 1.0f) curColor.m_Alpha = 1.0f;
					else if(curColor.m_Alpha < 0.0f) curColor.m_Alpha = 0.0f;
				}
				for(SprFrame::LayerList::iterator i = ((SprFrame*)*m_CurrentFrame)->m_LayerList.begin(); i != ((SprFrame*)*m_CurrentFrame)->m_LayerList.end(); i++)
					i->DrawLayer(m_Position + dstPos, srcPos, curColor);
			}
			if(m_bSmoothMode)
			{
				if(IN_RANGE(m_NextFrame, m_FrameList))
				{
					Color nextColor = dstColor;
					nextColor.m_Alpha = nextColor.m_Alpha * (1.0f - GetWeight());
					for(SprFrame::LayerList::iterator i = ((SprFrame*)*m_NextFrame)->m_LayerList.begin(); i != ((SprFrame*)*m_NextFrame)->m_LayerList.end(); i++)
						i->DrawLayer(m_Position + dstPos, srcPos, nextColor);
				}
			}
		}
	}

	void Sprite::DrawWithChildren(const Point& dstPos, const Point& srcPos, const Color& color)
	{
		DrawSprite(dstPos, srcPos, color);

		for(NodeList::iterator i = m_ChildrenList.begin(); i != m_ChildrenList.end(); i++)
		{
			if(GetVisible())
				((Sprite*)*i)->DrawWithChildren(m_Position + dstPos, Point(), m_Color * color);
		}
	}

	void Sprite::AdvanceWithChildren(float deltaTime)
	{
		AdvanceFrame(deltaTime);
		NodeList tmpList = m_ChildrenList;
		for(NodeList::iterator i = tmpList.begin(); i != tmpList.end(); i++)
		{
			((Sprite*)*i)->AdvanceWithChildren(deltaTime);
		}
		if(m_DetachFlag == APNODE_DETACH) DetachSelf();
		else if(m_DetachFlag == APNODE_REMOVE) delete this;
	}

	Sprite* Sprite::GetMouseFocus(const Point& point)
	{
		for(NodeList::reverse_iterator i = m_ChildrenList.rbegin(); i != m_ChildrenList.rend(); i++)
		{
			if(Sprite* res = ((Sprite*)*i)->GetMouseFocus(point - m_Position))
				return res;
		}

		if(IN_RANGE(m_CurrentFrame, m_FrameList) && GetVisible())
		{
			for(SprFrame::LayerList::iterator j = ((SprFrame*)*m_CurrentFrame)->m_LayerList.begin(); j != ((SprFrame*)*m_CurrentFrame)->m_LayerList.end(); j++)
				if(j->m_Hitbox.Hit(point - m_Position)) return this;
		}
		return NULL;
	}

	bool Sprite::GetVisible() const
	{
		if(m_bVisible)
		{
			if(m_Color.GetAlpha() > 0.003f) return true;
		}
		return false;
	}

	Sprite& Sprite::operator = (const Sprite& spr)
	{
		if(&spr == this) return *this;
		SetName(spr.GetName());
		m_bVisible = spr.m_bVisible;
		m_Position = spr.m_Position;
		m_Color = spr.m_Color;

		for(FrmList::const_iterator i = spr.m_FrameList.begin(); i != spr.m_FrameList.end(); i++)
		{
			SprFrame* sf = new SprFrame;
			*sf = *((SprFrame*)*i);
			m_FrameList.push_back(sf);
		}
		Goto(0);

		m_Speed = spr.m_Speed;
		m_Activity = spr.m_Activity;
		m_PlayMode = spr.m_PlayMode;
		m_GotoMap = spr.m_GotoMap;

		return *this;
	}

	void Sprite::GetUsedTextures(std::vector<Texture*>& texList)
	{
		TexMap mTexMap;
		for(KeyFrmIter kfIter = m_FrameList.begin(); kfIter != m_FrameList.end(); kfIter++)
		{
			SprFrame* pSprFrm = (SprFrame*)*kfIter;
			for(SprFrame::LayerList::iterator lyIter = pSprFrm->m_LayerList.begin(); lyIter != pSprFrm->m_LayerList.end(); lyIter++)
			{
				if(!lyIter->m_pTexture) continue;
				mTexMap[lyIter->m_pTexture->GetName()] = lyIter->m_pTexture;
			}
		}
		for(TexMap::iterator texIter = mTexMap.begin(); texIter != mTexMap.end(); texIter++)
			texList.push_back(texIter->second);
	}


#define SAVE_STR(_s) (fwrite((_s)(), sizeof(TCHAR) * ((_s).GetLength() + 1), 1, fp))
#define SAVE_QUAD(_q)\
{\
	posX = (_q).m_lt.x;\
	posY = (_q).m_lt.y;\
	fwrite(&posX, sizeof(float), 1, fp);\
	fwrite(&posY, sizeof(float), 1, fp);\
	posX = (_q).m_rt.x;\
	posY = (_q).m_rt.y;\
	fwrite(&posX, sizeof(float), 1, fp);\
	fwrite(&posY, sizeof(float), 1, fp);\
	posX = (_q).m_rb.x;\
	posY = (_q).m_rb.y;\
	fwrite(&posX, sizeof(float), 1, fp);\
	fwrite(&posY, sizeof(float), 1, fp);\
	posX = (_q).m_lb.x;\
	posY = (_q).m_lb.y;\
	fwrite(&posX, sizeof(float), 1, fp);\
	fwrite(&posY, sizeof(float), 1, fp);\
}
#define SAVE_RECT(_r)\
{\
	posX = (_r).m_Left;\
	posY = (_r).m_Top;\
	fwrite(&posX, sizeof(float), 1, fp);\
	fwrite(&posY, sizeof(float), 1, fp);\
	posX = (_r).m_Right;\
	posY = (_r).m_Bottom;\
	fwrite(&posX, sizeof(float), 1, fp);\
	fwrite(&posY, sizeof(float), 1, fp);\
}
	bool Sprite::SaveSprite(const String& filename)
	{
		if(FILE* fp = _tfopen(filename(), _T("wb")))
		{
			// д���ļ���־
			char flag[] = "Sprite2";
			fwrite(flag, 7, 1, fp);
			// ����
			SAVE_STR(GetName());
			// �ɼ���
			fwrite(&m_bVisible, sizeof(bool), 1, fp);
			// ƽ������
			fwrite(&m_bSmoothMode, sizeof(bool), 1, fp);
			// ����(���ڸ��ڵ�)
			float posX = m_Position.x;
			float posY = m_Position.y;
			fwrite(&posX, sizeof(float), 1, fp);
			fwrite(&posY, sizeof(float), 1, fp);
			// ��ɫƫ��
			dword color = m_Color.GetColor();
			fwrite(&color, sizeof(dword), 1, fp);
			// �ٶ�
			fwrite(&m_Speed, sizeof(float), 1, fp);
			// �ɻ��
			fwrite(&m_Activity, sizeof(bool), 1, fp);
			// ����˳��
			fwrite(&m_PlayMode, sizeof(bool), 1, fp);

			// ����
			SAVE_STR(m_Remarks);

			//////////////////////////////////////////////////////////////////////////
			// ��������ӳ��
			TexMap mTexMap;
			for(KeyFrmIter kfIter = m_FrameList.begin(); kfIter != m_FrameList.end(); kfIter++)
			{
				SprFrame* pSprFrm = (SprFrame*)*kfIter;
				for(SprFrame::LayerList::iterator lyIter = pSprFrm->m_LayerList.begin(); lyIter != pSprFrm->m_LayerList.end(); lyIter++)
				{
					if(!lyIter->m_pTexture) continue;
					mTexMap[lyIter->m_pTexture->GetName()] = lyIter->m_pTexture;
				}
			}
			size_t numTextures = mTexMap.size();
			fwrite(&numTextures, sizeof(size_t), 1, fp);
			for(TexMap::iterator texIter = mTexMap.begin(); texIter != mTexMap.end(); texIter++)
			{
				SAVE_STR(texIter->second->GetName());
				SAVE_STR(texIter->second->GetFileName());
				Color* colKey = texIter->second->GetColorKey();
				bool bColKey = colKey ? true : false;
				fwrite(&bColKey, sizeof(bool), 1, fp);
				if(bColKey)
				{
					dword dwColKey = colKey->GetColor();
					fwrite(&dwColKey, sizeof(dword), 1, fp);
				}
				dword dwCrtFlags = texIter->second->GetCreateFlags();
				fwrite(&dwCrtFlags, sizeof(dword), 1, fp);
			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// ��תӳ��
			size_t numGotoMaps = m_GotoMap.size();
			fwrite(&numGotoMaps, sizeof(size_t), 1, fp);
			for(GotoMap::iterator g = m_GotoMap.begin(); g != m_GotoMap.end(); g++)
			{
				// ��ת����
				SAVE_STR(g->first);
				// ֡��
				fwrite(&g->second, sizeof(dword), 1, fp);
			}
			//////////////////////////////////////////////////////////////////////////
			// ֡����
			size_t numFrames = m_FrameList.size();
			fwrite(&numFrames, sizeof(size_t), 1, fp);
			for(KeyFrmIter i = m_FrameList.begin(); i != m_FrameList.end(); i++)
			{
				SprFrame* pSprFrm = (SprFrame*)*i;
				// �ȴ���ʱ��
				fwrite(&pSprFrm->m_WaitTime, sizeof(float), 1, fp);
				// ֹͣ
				fwrite(&pSprFrm->m_bStop, sizeof(bool), 1, fp);
				// ��ת
				fwrite(&pSprFrm->m_Goto, sizeof(dword), 1, fp);

				// �����
				size_t numLayers = pSprFrm->m_LayerList.size();
				fwrite(&numLayers, sizeof(size_t), 1, fp);
				for(SprFrame::LayerList::iterator j = pSprFrm->m_LayerList.begin(); j != pSprFrm->m_LayerList.end(); j++)
				{
					Layer* pLayer = &(*j);
					if(!pLayer->m_pTexture) continue;
					// ����ָ��
					SAVE_STR(pLayer->m_pTexture->GetName());
					// Դȡͼ����
					SAVE_QUAD(pLayer->m_SrcQuad);
					// Ŀ���ͼ����
					SAVE_QUAD(pLayer->m_DestQuad);
					// ��ɫƫ��
					for(ushort c = 0; c < 4; c++)
					{
						color = pLayer->m_Color[c].GetColor();
						fwrite(&color, sizeof(dword), 1, fp);
					}
					// ������
					fwrite(&pLayer->m_LayerIndex, sizeof(dword), 1, fp);
					// ��ײ��
					SAVE_RECT(pLayer->m_Hitbox);
				}
			}
			fclose(fp);
			return true;
		}
		return false;
	}

#define LOAD_QUAD(_q)\
{\
	file->Read(&(_q).m_lt.x, sizeof(float));\
	file->Read(&(_q).m_lt.y, sizeof(float));\
	file->Read(&(_q).m_rt.x, sizeof(float));\
	file->Read(&(_q).m_rt.y, sizeof(float));\
	file->Read(&(_q).m_rb.x, sizeof(float));\
	file->Read(&(_q).m_rb.y, sizeof(float));\
	file->Read(&(_q).m_lb.x, sizeof(float));\
	file->Read(&(_q).m_lb.y, sizeof(float));\
}
#define LOAD_RECT(_r)\
{\
	file->Read(&(_r).m_Left, sizeof(float));\
	file->Read(&(_r).m_Top, sizeof(float));\
	file->Read(&(_r).m_Right, sizeof(float));\
	file->Read(&(_r).m_Bottom, sizeof(float));\
}
	bool Sprite::LoadSprite(const String& filename)
	{
		RenderSystem* rs = RenderSystem::GetSingletonPtr();
		if(!rs) return false;

		File* file = FileSystem::GetSingleton().Open(filename);
		if(!file) return false;

		// �ļ����
		char flag[7];
		file->Read(flag, 7);
		if(!memcmp(flag, "Sprite2", 7))
		{
			// ����
			String strName;
			file->ReadString(strName);
			SetName(strName);
			// �ɼ���
			file->Read(&m_bVisible, sizeof(bool));
			// ƽ������
			file->Read(&m_bSmoothMode, sizeof(bool));
			// ����(���ڸ��ڵ�)
			file->Read(&m_Position.x, sizeof(float));
			file->Read(&m_Position.y, sizeof(float));
			// ��ɫƫ��
			dword color;
			file->Read(&color, sizeof(dword));
			m_Color = color;
			// �ٶ�
			file->Read(&m_Speed, sizeof(float));
			// �ɻ��
			file->Read(&m_Activity, sizeof(bool));
			// ����˳��
			file->Read(&m_PlayMode, sizeof(bool));

			// ����
			file->ReadString(m_Remarks);

			//////////////////////////////////////////////////////////////////////////
			// ��������ӳ��
			size_t numTextures;
			file->Read(&numTextures, sizeof(size_t));
			for(dword texIter = 0; texIter < numTextures; texIter++)
			{
				String strTexName;
				String strFileName;
				bool bColKey;
				Color colKey;
				file->ReadString(strTexName);
				file->ReadString(strFileName);
				file->Read(&bColKey, sizeof(bool));
				if(bColKey)
				{
					dword dwColKey;
					file->Read(&dwColKey, sizeof(dword));
					colKey = dwColKey;
				}
				dword dwCrtFlags;
				file->Read(&dwCrtFlags, sizeof(dword));

				if(!rs->TextureMgr.IsDefined(strTexName))
				{
					Texture* tex = new Texture;
					tex->SetName(strTexName);
					if(!tex->LoadFromFile(strFileName, bColKey ? &colKey : NULL, dwCrtFlags))
					{
						LOG(_T("LoadSprite: �޷���������"), Insane);
					}
					rs->TextureMgr.Add(tex);
				}
			}
			//////////////////////////////////////////////////////////////////////////
			// ��תӳ��
			size_t numGotoMaps;
			file->Read(&numGotoMaps, sizeof(size_t));
			for(dword g = 0; g < numGotoMaps; g++)
			{
				// ��ת����
				String strGotoLabel;
				file->ReadString(strGotoLabel);
				// ֡��
				dword dwFrmID;
				file->Read(&dwFrmID, sizeof(dword));
				m_GotoMap[strGotoLabel] = dwFrmID;
			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// ֡����
			size_t numFrames;
			file->Read(&numFrames, sizeof(size_t));
			for(dword i = 0; i < numFrames; i++)
			{
				SprFrame* pSprFrm = new SprFrame;
				// �ȴ���ʱ��
				file->Read(&pSprFrm->m_WaitTime, sizeof(float));
				// ֹͣ
				file->Read(&pSprFrm->m_bStop, sizeof(bool));
				// ��ת
				file->Read(&pSprFrm->m_Goto, sizeof(dword));

				// �����
				size_t numLayers;
				file->Read(&numLayers, sizeof(size_t));
				for(dword j = 0; j < numLayers; j++)
				{
					Layer mLayer;
					// ����ָ��
					String strTexName;
					file->ReadString(strTexName);
					mLayer.m_pTexture = rs->TextureMgr.Get(strTexName);
					// Դȡͼ����
					LOAD_QUAD(mLayer.m_SrcQuad);
					// Ŀ���ͼ����
					LOAD_QUAD(mLayer.m_DestQuad);
					// ��ɫƫ��
					for(ushort c = 0; c < 4; c++)
					{
						file->Read(&color, sizeof(dword));
						mLayer.m_Color[c] = color;
					}
					// ������
					file->Read(&mLayer.m_LayerIndex, sizeof(dword));
					// ��ײ��
					LOAD_RECT(mLayer.m_Hitbox);

					pSprFrm->AddLayer(mLayer);
				}
				AddFrame(pSprFrm);
			}
			FileSystem::GetSingleton().Close(file);
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Y����
	bool Sprite::Swap_Y(NodeList::iterator last, NodeList::iterator next)
	{
		if(IN_RANGE(last, m_Parent->m_ChildrenList) && IN_RANGE(next, m_Parent->m_ChildrenList))
		{
			Sprite* pSprLast = (Sprite*)*last;
			Sprite* pSprNext = (Sprite*)*next;

			if(pSprLast->m_Position.y > pSprNext->m_Position.y)
			{
				*last = pSprNext;
				*next = pSprLast;
				return true;
			}
		}
		return false;
	}

	bool Sprite::Sort_Y()
	{
		if(m_Parent)
		{
			for(NodeList::iterator i = m_Parent->m_ChildrenList.begin(); i != m_Parent->m_ChildrenList.end(); i++)
			{
				if(*i == this)	// �ҵ��Լ���λ��
				{
					bool bFlag = false;
					NodeList::iterator curIter = i;

					while(curIter != m_Parent->m_ChildrenList.begin() && Swap_Y(curIter - 1, curIter))
					{
						curIter--;
						bFlag = true;
					}

					if(!bFlag)
					{
						while(Swap_Y(curIter, curIter + 1)) curIter++;
					}
					return true;
				}
			}
		}
		return false;
	}
}