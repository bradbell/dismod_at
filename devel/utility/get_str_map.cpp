// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_str_map$$
$spell
   const
   str
   std
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Get a Value from a Const String to String Map$$

$head Syntax$$
$icode%value% = get_str_map(%std_map%, %key%)
%$$
$icode%value% = get_str_map(%std_map%, %key%, %default_value%)
%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_GET_STR_MAP_2%// END_GET_STR_MAP_2%1%$$
$srcthisfile%0%// BEGIN_GET_STR_MAP_3%// END_GET_STR_MAP_3%1%$$

$head std_map$$
is the map.

$head key$$
is the key for the key-value pair that we are accessing.

$head default_value$$
If $icode default_value$$ is present, it is the default value to use if the
key is not in the map.
If $icode default_value$$ is not present, an assert is generated if the
key is not in the map.

$head value$$
is the value corresponding to this key.

$end
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
