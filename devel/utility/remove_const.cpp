// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <dismod_at/remove_const.hpp>

// components of remove_const that do not depend on a template parameter

namespace dismod_at {
	remove_const::remove_const(
		CppAD::vector<double> lower_bound ,
		CppAD::vector<double> upper_bound )
	:
	lower_bound_(lower_bound)     ,
	upper_bound_(upper_bound)     ,
	n_both_( lower_bound.size() )
	{	assert( lower_bound.size() == upper_bound.size() );
		both2var_index_.resize(n_both_);
		//
		// both2var_index_, var2both_index_
		for(size_t i = 0; i < n_both_; ++i)
		{	if( lower_bound[i] == upper_bound[i] )
				both2var_index_[i] = n_both_;
			else
			{	both2var_index_[i] = var2both_index_.size();
				var2both_index_.push_back(i);
			}
		}
	}
	size_t remove_const::n_both(void) const
	{	return n_both_; }
	//
	size_t remove_const::n_const(void) const
	{	return n_both_ - var2both_index_.size(); }
	//
	size_t remove_const::n_var(void) const
	{	return var2both_index_.size(); }
	//
	const CppAD::vector<double>& remove_const::lower(void) const
	{	return lower_bound_; }
	//
	const CppAD::vector<double>& remove_const::upper(void) const
	{	return upper_bound_; }
	//
	const CppAD::vector<size_t>& remove_const::var2both_index(void) const
	{	return var2both_index_; }
	//
	const CppAD::vector<size_t>& remove_const::both2var_index(void) const
	{	return both2var_index_; }
}
