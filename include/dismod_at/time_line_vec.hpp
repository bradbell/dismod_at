// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_TIME_LINE_VEC_HPP
# define DISMOD_AT_TIME_LINE_VEC_HPP

# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float>
class time_line_vec {
public:
	// structure corresponding to one point in a time line
	struct time_point {double time; Float value; };
private:
	// fixed age grid for numerical averages w.r.t. age and time
	const CppAD::vector<double> age_grid_;
	//
	// Set by specialize to include all the points in age_grid_ as well as
	// age_lower and  age_upper
	CppAD::vector<double> extend_grid_;
	//
	// Set by specialize to subset of extend_grid_ that is between
	// age_lower and age_upper inclusive.
	// sub_grid_[0] is age_lower, age_grid_[ sub_grid_.size()-1 ] is age_upper.
	CppAD::vector<double> sub_grid_;
	//
	// Set by specialize
	double time_lower_;
	double time_upper_;
	//
	// vec_ has the same size as sub_grid_ and
	// vec_[j] contains the line for sub_grid_[j]
	CppAD::vector< CppAD::vector<time_point> >  vec_;
	//
	// near_equal
	static bool near_equal(double x, double y);
public:
	// constructor
	time_line_vec(const CppAD::vector<double>& age_grid);
	//
	// specialize
	void specialize(
		const double& age_lower  ,
		const double& age_upper  ,
		const double& time_lower ,
		const double& time_upper
	);
	// extend_grid
	const CppAD::vector<double>& extend_grid(void) const;
	//
	// sub_grid
	const CppAD::vector<double>& sub_grid(void) const;
	//
	// add_point
	void add_point(
		const size_t&     age_index ,
		const time_point& point
	);
	// time_line
	const CppAD::vector<time_point>& time_line(
		const size_t& age_index
	) const;
	//
	// age_time_avg
	Float age_time_avg(void) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
