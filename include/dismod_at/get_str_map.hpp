// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
