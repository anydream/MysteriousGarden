


#include "SceneManager.h"

namespace AnyPlay
{
	template<> SceneManager* Singleton<SceneManager>::m_Singleton = 0;

	SceneManager::SceneManager()
	{
		Clear();

		InputSystem::GetSingleton().SubscribeEvent(KeyEvent, SubscriberSlot(&SceneManager::OnKey, this));
		InputSystem::GetSingleton().SubscribeEvent(MotionEvent, SubscriberSlot(&SceneManager::OnMotion, this));
	}

	void SceneManager::Clear()
	{
		m_pRoot = NULL;
		m_pFocus = NULL;
		m_pMouseFocus = NULL;
		m_bDragging = false;
	}

	bool SceneManager::OnKey(const EventArgs& args)
	{
		KeyEventArgs& ka = (KeyEventArgs&)args;
		InputSystem* is = InputSystem::GetSingletonPtr();

		if(is)
		{
			if(ka.m_VirtualKey == 0x1)	// ������
			{
				if(ka.m_bState)			// �������
				{
					// ���������ԭ����, ����½���
					if(m_pMouseFocus != m_pFocus)
					{
						if(m_pFocus) m_pFocus->FireEvent(LostFocusEvent, args);
						if(m_pMouseFocus) m_pMouseFocus->FireEvent(GetFocusEvent, args);
					}
					m_pFocus = m_pMouseFocus;
					if(m_pFocus) m_pFocus->FireEvent(MouseDownEvent, args);
				}
				else
				{
					if(m_pMouseFocus) m_pMouseFocus->FireEvent(MouseUpEvent, args);
					if(m_pFocus && m_pFocus == m_pMouseFocus) m_pFocus->FireEvent(MouseClickEvent, args);
				}
			}
			else
			{
				if(m_pFocus) m_pFocus->FireEvent(KeyEvent, args);
			}
		}

		return true;
	}

	bool SceneManager::OnMotion(const EventArgs& args)
	{
		MotionEventArgs& ma = (MotionEventArgs&)args;
		InputSystem* is = InputSystem::GetSingletonPtr();

		if(is && m_pRoot)
		{
			if(!m_bDragging)	// ����ҷ״̬
			{
				// ��õ�ǰ��겶�����
				Sprite* pCatched = m_pRoot->GetMouseFocus(is->GetMousePos());

				if(m_pMouseFocus != pCatched)	// ��ǰ��겶׽������ԭ��꽹��, �������꽹��
				{
					if(m_pMouseFocus) m_pMouseFocus->FireEvent(LostMouseFocusEvent, args);
					if(pCatched) pCatched->FireEvent(GetMouseFocusEvent, args);
					m_pMouseFocus = pCatched;
				}
				if(pCatched) pCatched->FireEvent(MouseMoveEvent, args);
			}
			else	// ��ҷ����
			{
				if(m_pMouseFocus) m_pMouseFocus->FireEvent(DraggingEvent, args);
			}
		}

		return true;
	}

	void SceneManager::AdvanceScene(float deltaTime)
	{
		if(m_pRoot) m_pRoot->AdvanceWithChildren(deltaTime);
	}

	void SceneManager::DrawScene()
	{
		if(m_pRoot) m_pRoot->DrawWithChildren(Point(0, 0));
	}
}