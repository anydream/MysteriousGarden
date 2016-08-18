/************************************************************************/
// ����ʱ��: 081029
// ���ȷ��: 081121

#ifndef _Sprite_h_
#define _Sprite_h_

#include "../AnyPlay.h"
#include "../Core/Animation.h"
#include "../Core/Node.h"
#include "../Core/String.h"
#include "../Core/Color.h"
#include "../Core/Quad.h"
#include "../RenderSystem/Texture.h"

namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// ��
	class Layer
	{
	public:
		Layer() : m_pTexture(NULL), m_LayerIndex(0) {}
		Layer(Texture* pTex, const Quad& srcQuad, const Quad& dstQuad, const Color& color = Color(), const Rect& hitBox = Rect()) :
		m_pTexture(pTex), m_SrcQuad(srcQuad), m_DestQuad(dstQuad), m_LayerIndex(0), m_Hitbox(hitBox)
		{
			m_Color[0] = color;
			m_Color[1] = color;
			m_Color[2] = color;
			m_Color[3] = color;
		}

		// ���Ʋ�
		bool DrawLayer(const Point& dstOff, const Point& srcOff, const Color& color = Color());

	public:
		Texture*	m_pTexture;		// ����ָ��
		Quad		m_SrcQuad;		// Դȡͼ����
		Quad		m_DestQuad;		// Ŀ���ͼ����
		Color		m_Color[4];		// ��ɫƫ��
		dword		m_LayerIndex;	// ������
		Rect		m_Hitbox;		// ��ײ��
	};

	//////////////////////////////////////////////////////////////////////////
	// ����֡
	class SprFrame : public KeyFrame
	{
	public:
		SprFrame() {}
		SprFrame(float wt) { m_WaitTime = wt; }
		SprFrame(const Layer& ly, float wt) { m_WaitTime = wt; m_LayerList.push_back(ly); }

		void AddLayer(const Layer& ly, dword pos = -1)
		{
			if(pos == -1) m_LayerList.push_back(ly);
			else m_LayerList.insert(m_LayerList.begin() + pos, ly);
		}

		void DeleteLayer(dword pos = -1)
		{
			if(pos == -1) m_LayerList.pop_back();
			else m_LayerList.erase(m_LayerList.begin() + pos);
		}

		void	DeleteAllLayers() { m_LayerList.clear(); }

		size_t	GetNumLayers() const { return m_LayerList.size(); }

	public:
		typedef std::vector<Layer> LayerList;
		LayerList	m_LayerList;
		// Shape	m_Shape;
	};

	//////////////////////////////////////////////////////////////////////////
	// ����
	class Sprite : public Animation
	{
	public:
		Sprite() : m_bVisible(true), m_bSmoothMode(false) {}
		Sprite(const Sprite& spr) : m_bVisible(true), m_bSmoothMode(false) { *this = spr; }
		virtual ~Sprite() {}

		// ���ƾ���
		virtual void	DrawSprite(const Point& dstPos, const Point& srcPos = Point(), const Color& color = Color());

		// ���ư����ӽڵ����ڵľ���
		void			DrawWithChildren(const Point& dstPos, const Point& srcPos = Point(), const Color& color = Color());

		void			AdvanceWithChildren(float deltaTime);
		// ���������ղ�׽�ľ���ָ��
		virtual Sprite*	GetMouseFocus(const Point& point);

		// ���, ���ÿɼ���
		void			SetVisible(bool bVisible) { m_bVisible = bVisible; }
		bool			GetVisible() const;

		// ���, ��������
		void			SetPosition(const Point& pos) { m_Position = pos; }
		const Point&	GetPosition() const { return m_Position; }

		// ���, ����ƽ������
		void			SetSmoothMode(bool bMode) { m_bSmoothMode = bMode; }
		bool			GetSmoothMode() const { return m_bSmoothMode; }

		// ���, ������ɫ
		void			SetColor(const Color& col) { m_Color = col; }
		const Color&	GetColor() const { return m_Color; }

		// ���, ���ø���
		void			SetRemarks(const String& remarks) { m_Remarks = remarks; }
		const String&	GetRemarks() const { return m_Remarks; }

		// ������õ������б�
		void			GetUsedTextures(std::vector<Texture*>& texList);

		Sprite&			operator = (const Sprite& spr);

		bool			SaveSprite(const String& filename);
		bool			LoadSprite(const String& filename);

		bool			Sort_Y();

	private:
		bool			Swap_Y(NodeList::iterator last, NodeList::iterator next);

		typedef std::map<String, Texture*, String::FastLessCompare> TexMap;
	protected:
		bool			m_bVisible;		// �ɼ���
		bool			m_bSmoothMode;	// ƽ��ģʽ
		Point			m_Position;		// ����(���ڸ��ڵ�)
		Color			m_Color;		// ��ɫƫ��
		String			m_Remarks;		// ����
	};
}
#endif