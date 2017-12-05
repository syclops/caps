//
// Created by smaptas on 29.11.17.
//

#ifndef CAPS_DOMAIN_MAP_H
#define CAPS_DOMAIN_MAP_H

// Include C++ standard library headers.
#include <fstream>
#include <map>
#include <string>
#include <vector>

// Include headers from other projects.
#include <boost/date_time/gregorian/gregorian.hpp>

enum class Status
{
  absent = 0,
  present = 1
};

std::string base_date(std::string filename)
{
  auto base_start_pos = filename.rfind('/');
  if (base_start_pos == std::string::npos) {
    return "";
  }
  return std::string(filename, base_start_pos + 1, 8);
}

template <typename Sequencer>
struct diff_trait
{
  using type = Sequencer;
  using diff = Sequencer;

  static size_t size(diff value)
  {
    return static_cast<size_t>(value);
  }

  static const type type_default;
};

template <typename Sequencer>
const typename diff_trait<Sequencer>::type diff_trait<Sequencer>::type_default{};

template <>
const typename diff_trait<int>::type diff_trait<int>::type_default{0};

template <>
const typename diff_trait<float>::type diff_trait<float>::type_default{0};

namespace bg = boost::gregorian;

template<>
struct diff_trait<bg::date>
{
  using type = bg::date;
  using diff = bg::date_duration;

  static size_t size(diff value)
  {
    return static_cast<size_t>(value.days());
  }

  static const type type_default;
};

const diff_trait<bg::date>::type diff_trait<bg::date>::type_default{};

template <typename Key, typename Sequencer>
class TimeMapBase
{
 public:
  TimeMapBase()
    : size_{0}, start_{diff_trait<Sequencer>::type_default}, length_{0}, map_{}
  {
    // Nothing to do here.
  }

  explicit TimeMapBase(typename diff_trait<Sequencer>::type start)
    : size_{0}, start_{start}, length_{0}, map_{}
  {
    // Nothing to do here.
  }

  virtual void add_element(typename diff_trait<Sequencer>::type point, Key key) = 0;

  virtual void pad() = 0;

  size_t size() const
  {
    return size_;
  }

  typename diff_trait<Sequencer>::diff length() const
  {
    return length_;
  }

  std::map<Key, std::vector<Status>> map() const
  {
    return map_;
  }

  typename diff_trait<Sequencer>::type start() const
  {
    return start_;
  }

 protected:
  size_t size_;
  typename diff_trait<Sequencer>::type start_;
  typename diff_trait<Sequencer>::diff length_;
  std::map<Key, std::vector<Status>> map_;
};

template <typename Key, typename Sequencer>
class TimeMap: public TimeMapBase<Key, Sequencer>
{
 public:
  void add_element(typename diff_trait<Sequencer>::type point, Key key) override
  {
    if (this->map_.find(key) == this->map_.end()) {
      this->map_.emplace(key, std::vector<Status>{});
    }
    if (this->start_ == diff_trait<Sequencer>::type_default) {
      this->start_ = point;
    }
    this->length_ = point - this->start_;
    while (this->map_[key].size()
           < diff_trait<Sequencer>::size(this->length_)) {
      this->map_[key].push_back(Status::absent);
    }
    this->map_[key].push_back(Status::present);
  }

  void pad() override
  {
    for (auto itr: this->map_) {
      while(this->map_[itr.first].size()
            < static_cast<size_t>(this->length_.days()) + 1) {
        this->map_[itr.first].push_back(Status::absent);
      }
    }
  }
};

class DomainMap: public TimeMap<std::string, bg::date>
{
 public:
  void add_files(const std::vector<std::string>& filenames)
  {
    for (const auto& name: filenames) {
      add_file_by_name(name);
    }
    pad();
  }

  void add_file_by_name(const std::string& filename)
  {
    auto file_date = bg::from_undelimited_string(base_date(filename));
    if (start_ == bg::date{bg::not_a_date_time}) {
      start_ = file_date;
    } else {
      length_ = file_date - start_;
    }
    std::ifstream in_stream(filename);
    std::string line;
    while (std::getline(in_stream, line)) {
      if (map_.find(line) == map_.end()) {
        map_.emplace(line, std::vector<Status>{});
      }
      while (map_[line].size() < static_cast<size_t>(length_.days())) {
        map_[line].push_back(Status::absent);
      }
      map_[line].push_back(Status::present);
    }
  }

  void pad()
  {
    for (auto itr: map_) {
      while(map_[itr.first].size() < static_cast<size_t>(length_.days()) + 1) {
        map_[itr.first].push_back(Status::absent);
      }
    }
  }
};



///**
// *
// * @tparam Key
// * @tparam Sequencer
// */
//template <typename Key, typename Sequencer>
//class SequenceMap
//{
// public:
//
//  explicit SequenceMap(diff_trait<Sequencer>::point start)
//    : size_{0}, length_{diff_trait<Sequencer>::zero}, map_{},
//      start_{std::move(start)}
//  {
//    // Nothing to do here.
//  }
//
//  void add_key(Key key)
//  {
//    // TODO
//  }
//
//  void step(diff_trait<Sequencer>::diff step_size = diff_trait<Sequencer>::step)
//  {
//    length_ += step_size;
//  }
//
//  size_t size() const
//  {
//    return size_;
//  }
//
//  diff_trait<Sequencer>::point current() const
//  {
//    return start_ + length_;
//  }
//
//  /**
//   * Get the sequence length of the map.
//   * @return - the length of the sequence.
//   */
//  diff_trait<Sequencer>::diff length() const
//  {
//    return length_;
//  }
//
// private:
//  size_t size_;
//  diff_trait<Sequencer>::diff length_;
//  std::map<Key, std::vector<Status>> map_;
//  diff_trait<Sequencer>::point start_;
//};

#endif //CAPS_DOMAIN_MAP_H
