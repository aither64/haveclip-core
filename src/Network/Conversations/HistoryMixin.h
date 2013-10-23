#ifndef HISTORYMIXIN_H
#define HISTORYMIXIN_H

class History;

namespace Conversations {
	class HistoryMixin
	{
	public:
		History *history();
		void setHistory(History *h);

	protected:
		History *m_history;
	};
}

#endif // HISTORYMIXIN_H
