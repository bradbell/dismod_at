// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_STR_MAP_HPP
# define DISMOD_AT_GET_STR_MAP_HPP

# include <map>
# include <string>

namespace dismod_at {
   const std::string& get_str_map(
         const std::map<std::string, std::string>& std_map ,
         const std::string&                        key
   );
   const std::string& get_str_map(
         const std::map<std::string, std::string>& std_map       ,
         const std::string&                        key           ,
         const std::string&                        default_value
   );
}

# endif
