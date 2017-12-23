#ifndef VISUALDL_LOGIC_SDK_H
#define VISUALDL_LOGIC_SDK_H

#include "visualdl/storage/storage.h"
#include "visualdl/storage/tablet.h"
#include "visualdl/utils/string.h"
namespace visualdl {

class Writer {
public:
  Writer(const std::string& mode, const std::string& dir) : mode_(mode) {
    storage_.SetDir(dir);
  }

  Tablet AddTablet(const std::string& tag) {
    // TODO(ChunweiYan) add string check here.
    auto tmp = mode_ + "/" + tag;
    string::TagEncode(tmp);
    return storage_.AddTablet(tmp);
  }

  Storage& storage() { return storage_; }

private:
  Storage storage_;
  std::string mode_;
};

class Reader {
public:
  Reader(const std::string& mode, const std::string& dir)
      : mode_(mode), reader_(dir) {}

  TabletReader tablet(const std::string& tag) {
    auto tmp = mode_ + "/" + tag;
    string::TagEncode(tmp);
    return reader_.tablet(tmp);
  }

private:
  StorageReader reader_;
  std::string mode_;
};

namespace components {

/*
 * Read and write support for Scalar component.
 */
template <typename T>
struct Scalar {
  Scalar(Tablet tablet) : tablet_(tablet) {
    tablet_.SetType(Tablet::Type::kScalar);
  }

  void SetCaption(const std::string cap) {
    tablet_.SetCaptions(std::vector<std::string>({cap}));
  }

  void AddRecord(int id, T value) {
    auto record = tablet_.AddRecord();
    record.SetId(id);
    auto entry = record.AddData<T>();
    entry.Set(value);
  }

private:
  Tablet tablet_;
};

template <typename T>
struct ScalarReader {
  ScalarReader(TabletReader&& reader) : reader_(reader) {}

  std::vector<T> records() const;
  std::vector<T> ids() const;
  std::vector<T> timestamps() const;
  std::vector<std::string> captions() const;
  size_t size() const;

private:
  TabletReader reader_;
};

template <typename T>
std::vector<T> ScalarReader<T>::records() const {
  std::vector<T> res;
  for (int i = 0; i < reader_.total_records(); i++) {
    res.push_back(reader_.record(i).data<T>(0).Get());
  }
  return res;
}

template <typename T>
std::vector<T> ScalarReader<T>::ids() const {
  std::vector<T> res;
  for (int i = 0; i < reader_.total_records(); i++) {
    res.push_back(reader_.record(i).id());
  }
  return res;
}

template <typename T>
std::vector<T> ScalarReader<T>::timestamps() const {
  std::vector<T> res;
  for (int i = 0; i < reader_.total_records(); i++) {
    res.push_back(reader_.record(i).timestamp());
  }
  return res;
}

template <typename T>
std::vector<std::string> ScalarReader<T>::captions() const {
  return reader_.captions();
}

template <typename T>
size_t ScalarReader<T>::size() const {
  return reader_.total_records();
}

}  // namespace components
}  // namespace visualdl

#endif
