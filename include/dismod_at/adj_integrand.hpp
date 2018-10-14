/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_ADJ_INTEGRAND_HPP
# define DISMOD_AT_ADJ_INTEGRAND_HPP

# include <cppad/utility/vector.hpp>
# include "get_integrand_table.hpp"
# include "pack_info.hpp"


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class adj_integrand {
private:
		const std::string&                        rate_case_;
		const CppAD::vector<double>&              age_table_;
		const CppAD::vector<double>&              time_table_;
		const CppAD::vector<integrand_struct>&    integrand_table_;
		const CppAD::vector<smooth_info>&         s_info_vec_;
		const pack_info&                          pack_object_;
public:
	adj_integrand(
		const std::string&                        rate_case        ,
		const CppAD::vector<double>&              age_table        ,
		const CppAD::vector<double>&              time_table       ,
		const CppAD::vector<integrand_struct>&    integrand_table  ,
		const CppAD::vector<smooth_info>&         s_info_vec       ,
		const pack_info&                          pack_object
	);
	template <class Float>
	CppAD::vector<Float> line(
		size_t                                    integrand_id     ,
		size_t                                    n_child          ,
		size_t                                    child            ,
		const CppAD::vector<double>&              x                ,
		const CppAD::vector<double>&              line_age         ,
		const CppAD::vector<double>&              line_time        ,
		const CppAD::vector<Float>&               pack_vec
	);
};

} // END_DISMOD_AT_NAMESPACE

# endif
