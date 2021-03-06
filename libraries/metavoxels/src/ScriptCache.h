//
//  ScriptCache.h
//  metavoxels
//
//  Created by Andrzej Kapolka on 2/4/14.
//  Copyright (c) 2014 High Fidelity, Inc. All rights reserved.
//

#ifndef __interface__ScriptCache__
#define __interface__ScriptCache__

#include <QHash>
#include <QList>
#include <QNetworkRequest>
#include <QObject>
#include <QScriptProgram>
#include <QScriptValue>
#include <QSharedPointer>
#include <QWeakPointer>

#include "MetavoxelUtil.h"

class QNetworkAccessManager;
class QNetworkReply;
class QScriptEngine;

class NetworkProgram;
class NetworkValue;

/// Maintains a cache of loaded scripts.
class ScriptCache : public QObject {
    Q_OBJECT

public:

    static ScriptCache* getInstance();

    ScriptCache();
    
    void setNetworkAccessManager(QNetworkAccessManager* manager) { _networkAccessManager = manager; }
    QNetworkAccessManager* getNetworkAccessManager() const { return _networkAccessManager; }
    
    void setEngine(QScriptEngine* engine);
    QScriptEngine* getEngine() const { return _engine; }
    
    /// Loads a script program from the specified URL.
    QSharedPointer<NetworkProgram> getProgram(const QUrl& url);

    /// Loads a script value from the specified URL.
    QSharedPointer<NetworkValue> getValue(const ParameterizedURL& url);

    const QScriptString& getParametersString() const { return _parametersString; }
    const QScriptString& getLengthString() const { return _lengthString; }
    const QScriptString& getNameString() const { return _nameString; }
    const QScriptString& getTypeString() const { return _typeString; }
    const QScriptString& getGeneratorString() const { return _generatorString; }

private:
    
    QNetworkAccessManager* _networkAccessManager;
    QScriptEngine* _engine;
    QHash<QUrl, QWeakPointer<NetworkProgram> > _networkPrograms;
    QHash<ParameterizedURL, QWeakPointer<NetworkValue> > _networkValues;
    QScriptString _parametersString;
    QScriptString _lengthString;
    QScriptString _nameString;
    QScriptString _typeString;
    QScriptString _generatorString;
};

/// A program loaded from the network.
class NetworkProgram : public QObject {
    Q_OBJECT

public:

    NetworkProgram(ScriptCache* cache, const QUrl& url);
    ~NetworkProgram();
    
    ScriptCache* getCache() const { return _cache; }
    
    bool isLoaded() const { return !_program.isNull(); }
    
    const QScriptProgram& getProgram() const { return _program; }

signals:

    void loaded();

private slots:
    
    void makeRequest();
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void handleReplyError(); 
        
private:
    
    ScriptCache* _cache;
    QNetworkRequest _request;
    QNetworkReply* _reply;
    int _attempts;
    QScriptProgram _program;
};

/// Abstract base class of values loaded from the network.
class NetworkValue {
public:
    
    virtual ~NetworkValue();
    
    bool isLoaded() { return getValue().isValid(); }

    virtual QScriptValue& getValue() = 0;

protected:
    
    QScriptValue _value;
};

/// Contains information about a script parameter.
class ParameterInfo {
public:
    QScriptString name;
    int type;
};

/// The direct result of running a program.
class RootNetworkValue : public NetworkValue {
public:
    
    RootNetworkValue(const QSharedPointer<NetworkProgram>& program);

    const QSharedPointer<NetworkProgram>& getProgram() const { return _program; }

    virtual QScriptValue& getValue();

    const QList<ParameterInfo>& getParameterInfo();

private:
    
    QSharedPointer<NetworkProgram> _program;
    QList<ParameterInfo> _parameterInfo;
};

/// The result of running a program's generator using a set of arguments.
class DerivedNetworkValue : public NetworkValue {
public:

    DerivedNetworkValue(const QSharedPointer<NetworkValue>& baseValue, const ScriptHash& parameters);

    virtual QScriptValue& getValue();
    
private:
    
    QSharedPointer<NetworkValue> _baseValue;
    ScriptHash _parameters;
};

#endif /* defined(__interface__ScriptCache__) */
