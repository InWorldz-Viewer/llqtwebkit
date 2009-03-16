/*
   Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
*/

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
 * The Original Code is Torch Mobile Inc. (http://www.torchmobile.com/) code
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

#include <QtCore/QtCore>
#include <trie_p.h>

QStringList generateKey() {
    QStringList key;
    int size = qrand() % 20 + 3;
    while (key.count() < size)
        key += QString(QChar::fromAscii(qrand() % 26 + 64));
    return key;
}

void basicCheck() {
    QStringList list;
    list << QLatin1String("to") << QLatin1String("tea") << QLatin1String("ten") << QLatin1String("i") << QLatin1String("in") << QLatin1String("inn");
    Trie<int> trie;
    for (int i = 0; i < list.count(); ++i) {
        QString key = list[i];
        QStringList keyList;
        for (int j = 0; j < key.count(); ++j)
            keyList.append(QString(key[j]));
        trie.insert(keyList, i);
    }
    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::ReadWrite);
        stream << trie;
    }
    Trie<int> trie2;
    {
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream >> trie2;
    }
    for (int i = 0; i < list.count(); ++i) {
        QString key = list[i];
        QStringList keyList;
        for (int j = 0; j < key.count(); ++j)
            keyList.append(QString(key[j]));
        QList<int> x = trie2.find(keyList);
        qDebug() << x.count() << i << x[0] << i;
        qDebug() << trie2.remove(keyList, i);
        qDebug() << trie2.find(keyList).count();
    }
}

int main(int argc, char **argv) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    basicCheck();

    QHash<QString, int> hash;
    Trie<int> t;
    while (hash.count() < 500) {
        qDebug() << hash.count();
        QStringList key = generateKey();
        int value = qrand() % 50000;
        hash[key.join(QLatin1String(","))] = value;
        t.insert(key, value);

        QHashIterator<QString, int> i(hash);
        while (i.hasNext()) {
            i.next();
            if (t.find(i.key().split(QLatin1Char(','))).count() == 0)
                qDebug() << i.key();
            Q_ASSERT(t.find(i.key().split(QLatin1Char(','))).count() > 0);
            if (qrand() % 500 == 0) {
                t.remove(i.key().split(QLatin1Char(',')), i.value());
                hash.remove(i.key());
            }
            //cout << i.key() << ": " << i.value() << endl;
        }
    }
    return 0;
}

