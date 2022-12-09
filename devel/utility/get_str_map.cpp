// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_str_map dev}

Get a Value from a Const String to String Map
#############################################

Syntax
******

| *value* = ``get_str_map`` ( *std_map* , *key* )
| *value* = ``get_str_map`` ( *std_map* , *key* , *default_value* )

Prototype
*********
{xrst_literal
   // BEGIN_GET_STR_MAP_2
   // END_GET_STR_MAP_2
}
{xrst_literal
   // BEGIN_GET_STR_MAP_3
   // END_GET_STR_MAP_3
}

std_map
*******
is the map.

key
***
is the key for the key-value pair that we are accessing.

default_value
*************
If *default_value* is present, it is the default value to use if the
key is not in the map.
If *default_value* is not present, an assert is generated if the
key is not in the map.

value
*****
is the value corresponding to this key.

{xrst_end get_str_map}
*/
# include <cassert>
# include <dismod_at/get_str_map.hpp>
namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_GET_STR_MAP_2
const std::string& get_str_map(
   const std::map<std::string, std::string>&  std_map       ,
   const std::string&                         key           )
// END_GET_STR_MAP_2
{  //
   std::map<std::string, std::string>::const_iterator map_itr;
   map_itr = std_map.find(key);
   assert( map_itr != std_map.end() );
   return map_itr->second;
}

// BEGIN_GET_STR_MAP_3
const std::string& get_str_map(
   const std::map<std::string, std::string>&  std_map       ,
   const std::string&                         key           ,
   const std::string&                         default_value )
// END_GET_STR_MAP_3
{  //
   std::map<std::string, std::string>::const_iterator map_itr;
   map_itr = std_map.find(key);
   if( map_itr == std_map.end() )
      return default_value;
   return map_itr->second;
}

} // END DISMOD_AT_NAMESPACE
