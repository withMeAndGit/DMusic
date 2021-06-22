#pragma once
#include <atomic>
#include <QObject>
#include <QVariantList>
#include <QQmlEngine>
#include <QJSEngine>
#include <QAbstractListModel>
#include "python.hpp"
#include "api.hpp"

struct YArtist;
struct YClient;

struct YTrack : Track
{
  Q_OBJECT
public:
  YTrack(qint64 id, QObject* parent = nullptr);
  YTrack(py::object obj, QObject* parent = nullptr);
  ~YTrack();
  YTrack(QObject* parent = nullptr);

  PyObject* raw() const { return _py.raw; }

	QString idStr() override;
  QString title() override;
  QString artistsStr() override;
  QString extra() override;
  QString cover() override;
  QMediaContent media() override;
  qint64 duration() override;
  bool liked() override;

  qint64 id();
  bool available();
  QVector<YArtist> artists();
  File coverFile();
  File metadataFile();
  File mediaFile();

  QJsonObject jsonMetadata();
  QString stringMetadata();
  void saveMetadata();

public slots:
  void setLiked(bool liked) override;

private:
  bool _loadFromDisk();
  
  void _fetchYandex();
  void _fetchYandex(py::object _pys);
  void _downloadCover();
  void _downloadMedia();
  void _checkLiked();

  QString _coverUrl();

  py::object _py;
  QRecursiveMutex _mtx{};
  qint64 _id;
  QString _title, _author, _extra, _cover, _media;
  qint64 _duration;
  bool _liked = false;
  QVector<qint64> _artists;
  bool _noTitle = false, _noAuthor = false, _noExtra = false, _noCover = false, _noMedia = false;
  bool _hasLiked = false;
  bool _relativePathToCover = true;
  bool _checkedDisk = false;
};
inline PyObject* toPyObject(YTrack const& a) { Py_INCREF(a.raw()); return a.raw(); }
inline void fromPyObject(py::object const& o, YTrack*& res) { res = new YTrack(o, nullptr); }

struct YArtist : QObject
{
  Q_OBJECT
public:
  YArtist(py::object impl, QObject* parent = nullptr);
  YArtist();
  YArtist(YArtist const& copy);
  YArtist& operator=(YArtist const& copy);
  PyObject* raw() { return impl.raw; }

  int id();
  QString name();
  QString coverPath();
  QString metadataPath();

  QJsonObject jsonMetadata();
  QString stringMetadata();
  void saveMetadata();
  bool saveCover(int quality = 1000);

private:
  py::object impl;
  int _id;
};
inline PyObject* toPyObject(YArtist a) { Py_INCREF(a.raw()); return a.raw(); }
inline void fromPyObject(py::object const& o, YArtist& res) { res = YArtist(o.raw); }
inline void fromPyObject(py::object const& o, YArtist*& res) { res = new YArtist(o.raw); }

struct YPlaylist : QObject
{
  Q_OBJECT
public:
  YPlaylist(py::object impl, QObject* parent = nullptr);
  YPlaylist();
  PyObject* raw() const { return impl.raw; }

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QUrl cover READ cover WRITE setCover NOTIFY coverChanged)

  virtual QString name();
  virtual QUrl cover();
  virtual refPlaylist toPlaylist();

public slots:
  bool setName(QString name);
  bool setCover(QUrl cover);

signals:
  void nameChanged(QString name);
  void coverChanged(QUrl cover);

private:
  py::object impl;
};
inline PyObject* toPyObject(YPlaylist const& a) { Py_INCREF(a.raw()); return a.raw(); }
inline void fromPyObject(py::object const& o, YPlaylist*& res) { res = new YPlaylist(o.raw); }

struct YLikedTracks : YPlaylist
{
  Q_OBJECT
public:
  YLikedTracks(QObject* parent = nullptr);

  static YLikedTracks* instance;
  static YLikedTracks* qmlInstance(QQmlEngine*, QJSEngine*);

  QString name() override;
  QUrl cover() override;
  refPlaylist toPlaylist() override;
};

struct YPlaylistsModel : QAbstractListModel
{
  Q_OBJECT
public:
  YPlaylistsModel(QObject* parent = nullptr);
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QVector<YPlaylist*> playlists;
};

struct YClient : QObject
{
	Q_OBJECT
public:
  ~YClient();
  YClient(QObject *parent = nullptr);

  static YClient* instance;
  static YClient* qmlInstance(QQmlEngine*, QJSEngine*);

  Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)

  bool initialized();

  refTrack track(qint64 id);

public slots:
  bool isLoggined();

  void init();

  QString token(QString login, QString password);
  bool login(QString token);
  void login(QString token, QJSValue const& callback);
  bool loginViaProxy(QString token, QString proxy);
  void loginViaProxy(QString token, QString proxy, QJSValue const& callback);

  QVector<py::object> fetchTracks(qint64 id);

  YLikedTracks* likedTracks();
  YPlaylist* playlist(int id);
  Playlist* oneTrack(qint64 id);
  YPlaylist* userDailyPlaylist();
  Playlist* userTrack(int id);
  Playlist* downloadsPlaylist();
  YPlaylistsModel* homePlaylistsModel();

  void playPlaylist(YPlaylist* playlist);

  void addUserTrack(QString media, QString cover, QString title, QString artists, QString extra);

signals:
  void initializedChanged(bool initialized);

public:
  py::module ym; // yandex_music module
  py::module ym_request;

  py::object me; // client

private:
  bool _initialized = false;
  std::atomic_bool loggined = false;
};
