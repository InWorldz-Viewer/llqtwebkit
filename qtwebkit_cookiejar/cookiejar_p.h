/*
   Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
*/

#ifndef COOKIEJARPRIVATE_H
#define COOKIEJARPRIVATE_H

#include "trie_p.h"

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QNetworkCookie &cookie)
{
    stream << cookie.toRawForm();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, QNetworkCookie &cookie)
{
    QByteArray value;
    stream >> value;
    QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
    if (!newCookies.isEmpty())
        cookie = newCookies.first();
    return stream;
}
QT_END_NAMESPACE

class CookieJarPrivate {
public:
    CookieJarPrivate()
        : setSecondLevelDomain(false)
    {}

    Trie<QNetworkCookie> tree;
    mutable bool setSecondLevelDomain;
    mutable QStringList secondLevelDomains;

    bool matchesBlacklist(const QString &string) const;
    bool matchingDomain(const QNetworkCookie &cookie, const QUrl &url) const;
    QString urlPath(const QUrl &url) const;
    bool matchingPath(const QNetworkCookie &cookie, const QString &urlPath) const;
};

#endif

