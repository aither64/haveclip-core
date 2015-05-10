/*
  HaveClip

  Copyright (C) 2013-2015 Jakub Skokan <aither@havefun.cz>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ClipboardUpdateConfirm.h"

using namespace Commands;

ClipboardUpdateConfirm::ClipboardUpdateConfirm(ClipboardContainer *cont, Communicator::Role r)
	: BaseCommand(cont, r)
{
}

BaseCommand::Type ClipboardUpdateConfirm::type() const
{
	return BaseCommand::ClipboardUpdateConfirm;
}

void ClipboardUpdateConfirm::receive(QDataStream &ds)
{
	readStatus(ds);

	qint32 mode;

	ds >> mode;
	ds >> m_filters;

	m_mode = (Settings::MimeFilterMode) mode;

	finish();
}

void ClipboardUpdateConfirm::send(QDataStream &ds)
{
	writeStatus(ds, status() == Undefined ? Ok : status());

	ds << (qint32) m_mode;
	ds << m_filters;

	finish();
}

Settings::MimeFilterMode ClipboardUpdateConfirm::filterMode() const
{
	return m_mode;
}

const QStringList &ClipboardUpdateConfirm::filters() const
{
	return m_filters;
}

void ClipboardUpdateConfirm::setFilters(Settings::MimeFilterMode mode, const QStringList &filters)
{
	m_mode = mode;
	m_filters = filters;
}
