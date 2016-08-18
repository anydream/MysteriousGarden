/************************************************************************/
// 创建时间: 081029
// 最后确定: 081121

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
	// 层
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

		// 绘制层
		bool DrawLayer(const Point& dstOff, const Point& srcOff, const Color& color = Color());

	public:
		Texture*	m_pTexture;		// 纹理指针
		Quad		m_SrcQuad;		// 源取图坐标
		Quad		m_DestQuad;		// 目标绘图坐标
		Color		m_Color[4];		// 颜色偏向
		dword		m_LayerIndex;	// 层索引
		Rect		m_Hitbox;		// 碰撞盒
	};

	//////////////////////////////////////////////////////////////////////////
	// 精灵帧
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
	// 精灵
	class Sprite : public Animation
	{
	public:
		Sprite() : m_bVisible(true), m_bSmoothMode(false) {}
		Sprite(const Sprite& spr) : m_bVisible(true), m_bSmoothMode(false) { *this = spr; }
		virtual ~Sprite() {}

		// 绘制精灵
		virtual void	DrawSprite(const Point& dstPos, const Point& srcPos = Point(), const Color& color = Color());

		// 绘制包括子节点在内的精灵
		void			DrawWithChildren(const Point& dstPos, const Point& srcPos = Point(), const Color& color = Color());

		void			AdvanceWithChildren(float deltaTime);
		// 获得鼠标最终捕捉的精灵指针
		virtual Sprite*	GetMouseFocus(const Point& point);

		// 获得, 设置可见性
		void			SetVisible(bool bVisible) { m_bVisible = bVisible; }
		bool			GetVisible() const;

		// 获得, 设置坐标
		void			SetPosition(const Point& pos) { m_Position = pos; }
		const Point&	GetPosition() const { return m_Position; }

		// 获得, 设置平滑属性
		void			SetSmoothMode(bool bMode) { m_bSmoothMode = bMode; }
		bool			GetSmoothMode() const { return m_bSmoothMode; }

		// 获得, 设置颜色
		void			SetColor(const Color& col) { m_Color = col; }
		const Color&	GetColor() const { return m_Color; }

		// 获得, 设置附言
		void			SetRemarks(const String& remarks) { m_Remarks = remarks; }
		const String&	GetRemarks() const { return m_Remarks; }

		// 获得引用的纹理列表
		void			GetUsedTextures(std::vector<Texture*>& texList);

		Sprite&			operator = (const Sprite& spr);

		bool			SaveSprite(const String& filename);
		bool			LoadSprite(const String& filename);

		bool			Sort_Y();

	private:
		bool			Swap_Y(NodeList::iterator last, NodeList::iterator next);

		typedef std::map<String, Texture*, String::FastLessCompare> TexMap;
	protected:
		bool			m_bVisible;		// 可见性
		bool			m_bSmoothMode;	// 平滑模式
		Point			m_Position;		// 坐标(基于父节点)
		Color			m_Color;		// 颜色偏向
		String			m_Remarks;		// 附言
	};
}
#endif