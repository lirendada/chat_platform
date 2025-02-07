// file      : libbutl/string-table.hxx -*- C++ -*-
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>

#include <libbutl/multi-index.hxx>

#include <libbutl/export.hxx>

namespace butl
{
  // A pool of strings and, optionally, other accompanying data in which each
  // entry is assigned an individual index (or id) of type I (e.g., uint8_t,
  // uint16_t, etc., depending on how many entries are expected).  Index value
  // 0 is reserved to indicate the "no entry" condition.
  //
  template <typename I, typename D>
  struct string_table_element
  {
    const I i;
    const D d;
  };

  template <typename I>
  struct string_table_element<I, std::string>
  {
    const I i;
    const std::string d;
  };

  // For custom data the options are to call the data member 'key' or to
  // specialize this traits.
  //
  template <typename D>
  struct string_table_traits
  {
    static const std::string&
    key (const D& d) {return d.key;}
  };

  template <>
  struct string_table_traits<std::string>
  {
    static const std::string&
    key (const std::string& d) {return d;}
  };

  template <typename I, typename D = std::string>
  struct string_table
  {
    // Insert new entry unless one already exists.
    //
    I
    insert (const D&);

    // Find existing.
    //
    I
    find (const std::string& k) const
    {
      auto i (map_.find (key_type (&k)));
      return i != map_.end () ? i->second.i : 0;
    }

    // Reverse lookup.
    //
    const D&
    operator[] (I i) const {assert (i > 0); return vec_[i - 1]->second.d;}

    I
    size () const {return static_cast<I> (vec_.size ());}

    bool
    empty () const {return vec_.empty ();}

    void
    clear () {vec_.clear (); map_.clear ();}

  private:
    using key_type = butl::map_key<std::string>;
    using value_type = string_table_element<I, D>;
    using map_type = std::unordered_map<key_type, value_type>;
    using traits_type = string_table_traits<D>;

    map_type map_;
    std::vector<typename map_type::const_iterator> vec_;
  };
}

#include <libbutl/string-table.txx>
