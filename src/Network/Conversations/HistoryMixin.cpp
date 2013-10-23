#include "HistoryMixin.h"
#include "../../History.h"

using namespace Conversations;

History* HistoryMixin::history()
{
	return m_history;
}

void HistoryMixin::setHistory(History *h)
{
	m_history = h;
}
