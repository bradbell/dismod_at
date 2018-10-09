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
	// ages corresponding to vector of time lines
	// vec_age_[0]                    = age_lower in previous specialize
	// vec_age_[ vec_age_.size() - 1] = age_upper in previous speicalize
	CppAD::vector<double> vec_age_;
	//
	// lower and upper times corresponding to previous specialize
	double time_lower_;
	double time_upper_;
	//
	// vec_ has the same size as vec_age_ and
	// vec_[j] contains the line for vec_age_[j]
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
	// vec_age
	const CppAD::vector<double>& vec_age(void) const;
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
