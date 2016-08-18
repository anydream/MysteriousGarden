/************************************************************************/
// 创建时间: 081118
// 最后确定: 081125

#ifndef _NodeManager_h_
#define _NodeManager_h_

#include "../AnyPlay.h"
#include "../Core/Animation.h"
#include "../Core/Node.h"
#include "Singleton.h"
#include "Manager.h"

namespace AnyPlay
{
	class NodeManager : public Singleton<NodeManager>, public Manager<Node>
	{
	public:
		NodeManager() {}
	};
}

#endif