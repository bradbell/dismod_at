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
private:
	// fixed age grid for numerical averages w.r.t age and time
	const CppAD::vector<double> age_grid_;
	//
	// maximum time step for numerical averages w.r.t age and time
	const double                ode_time_step_;
	//
	// age grid corresponding to one average
	CppAD::vector<double>       this_age_grid_; 
	// 
	// vec_ has the same size as this_age_grid and
	// vec_[j] contains the line for this_age_grid_[j]
	struct {double time; Float value; }          time_point;
	CppAD::vector< CppAD::vector<time_point> >   vec_;
	//
	bool near_equal(double x, double y)
	{	// some constants for near equal calculations
		double eps99 = 99.0 * std::numeric_limits<double>::epsion();	
		double min99 = 99.0 * std::numeric_limits<double>::min();
		//
		double abs_x = std::fabs(x);
		double abs_y = std::fabs(y);
		//
		if( abs_x <= min99 && abs_y <= min99 )
			return true;
		//
		return std::abs(1.0 - x / y) <= eps99;
	}
public:
	// ctor
	time_line_vec(
		csont CppAD::vector<double>&   age_grid      ,
		const double&                  ode_time_step )
	:
	age_grid_(age_grid_),
	ode_time_step_(ode_time_step)
	{
# ifndef NDEBUG
		assert( 2 <= age_grid.size() ) 
		for(size_t j = 1; j < age_grid.size(); ++j)
			assert( age_grid[j-1] < age_grid[j] );
# endif
	}
	// initialize this_age_grid_ and vec_ for a new average
	void new_average(
		const double& age_lower  , 
		const double& age_upper  )
	{	assert( age_lower <= age_upper );
		assert( age_grid_[0] <= age_lower );
		assert( age_upper <= age_grid_[age_grid_.size() - 1] );
		//
		// initialize this age grid as empty
		this_age_grid_.resize(0);
		//
		// skip age grid values that are less than age_lower
		size_t j = 0;
		while( age_grid_[j] < age_lower )
			++j;
		//
		// check if age_lower is in age_grid
		bool found = near_equal( age_lower, age_grid[j] );
		if( not found )
		{	assert( age_lower < age_grid_[j] );
			this_age_grid_.push_back( age_lower );
		}
		//
		// include age_grid values in [ age_lower, age_upper ]
		while( age_grid_[j] < age_upper )
		{	this_age_grid_.push_back( age_grid_[j] );
			++j;
		}
		//
		// check if age_upper is in age_grid
		found = near_equal( age_upper, age_grid[j] );
		if( not found )
		{	assert( age_upper < age_grid_[j] );
			this_age_grid_.push_back( age_upper );
		}
		//
		// vec_
		size_t n_age = this_age_grid_.size();
		vec_.resize(n_age);
		for(size_t j = 0; j < n_age; ++j)
			vec_[j].resize(0);
	}
	// add one time_point
	void add_time_point(
		const size_t&     age_index ,
		const time_point& point     )
	{	// this time line
		CppAD::vector<time_point>& time_line = vec_[age_index];
		size_t n_time = time_line.size();
		//
		// time index at which to insert this point
		size_t time_index = 0;
		while( time_index < n_time && time_line[i].time < point.time )
			++time_index; 
		//
		// case where this point goes at the end of the line
		if( time_index == n_time )
		{	time_line.push_back(point);
			return;
		}
		//
		// case where this point is inserted at index i
		time_line.push_back( time_line[n_time - 1] );
		for(size_t i = n_time - 1; i >= time_index; --i)
			time_line[i+1] = time_line[i];
		time_line[time_index] = point;
		//
		return;
	}
	// return time line corresponding to an age index
	const CppAD::vector<time_point>& time_line(
		const size_t& age_index 
	) const
	{	return vec_[age_index]; }
};

} // END_DISMOD_AT_NAMESPACE

# endif
