/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Linden Lab Inc. (http://lindenlab.com) code.
 *
 * The Initial Developer of the Original Code is:
 *   Benjamin Meyer (benjamin.meyer@torchmobile.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "llstyle.h"

#include "llembeddedbrowserwindow_p.h"
#include <qstyleoption.h>
#include <qpainter.h>
#include <qdebug.h>

LLStyle::LLStyle()
    : QPlastiqueStyle()
{
}

void LLStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
#ifdef Q_WS_MAC
    if (control == QStyle::CC_ScrollBar) {
        if (static_cast<const LLWebView*>(widget)) {
            QStyleOptionSlider* opt = (QStyleOptionSlider*)option;
            const QPoint topLeft = opt->rect.topLeft();
            painter->translate(topLeft);
            opt->rect.moveTo(QPoint(0, 0));
            painter->fillRect(opt->rect, opt->palette.background());
        }
    }
#endif
    QPlastiqueStyle::drawComplexControl(control, option, painter, widget);
}
