// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_REMOVE_CONST_HPP
# define DISMOD_AT_REMOVE_CONST_HPP
# define DISMOD_AT_WEIGHT_INFO_HPP

# include <cppad/utility/vector.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class remove_const {
private:
	const CppAD::vector<double> lower_bound_;
	const CppAD::vector<double> upper_bound_;
	const size_t                n_both_;
	// effectevly const: set by constructor and does not change.
	size_t                      n_const_;
public:
	// 2DO: remove default constructor
	remove_const(void);
	//
	remove_const(
		CppAD::vector<double> lower_bound ,
		CppAD::vector<double> upper_bound
	);
	size_t n_both(void)  const;
	size_t n_const(void) const;
	size_t n_var(void)   const;
	CppAD::vector<double> remove(const CppAD::vector<double>& both) const;
	CppAD::vector<double> replace(const CppAD::vector<double>& both) const;

};

} // END_DISMOD_AT_NAMESPACE

# endif
