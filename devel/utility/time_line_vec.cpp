// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/time_line_vec.hpp>

/*
$begin time_line_vec$$
$spell
	vec
$$

$section Creating a Vector of Time Lines$$

$head Under Construction$$

$head Syntax$$
$codei%time_line_vec %vec%(%age_grid%)
%$$
$icode%vec%.initialize(%age_lower%, %age_upper%)
%$$
$icode%vec%.add_point(%index%, %point%)
%$$
$icode%vec_age% = %vec%.vec_age()
%$$
$icode%time_line% = %vec%.time_line(%index%)
%$$

$head Prototype$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_CONSTRUCTOR_PROTOTYPE%// END_CONSTRUCTOR_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_INITIALIZE_PROTOTYPE%// END_INITIALIZE_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_ADD_POINT_PROTOTYPE%// END_ADD_POINT_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_VEC_AGE_PROTOTYPE%// END_VEC_AGE_PROTOTYPE%1
%$$
$srcfile%devel/utility/time_line_vec.cpp%
	0%// BEGIN_TIME_LINE_PROTOTYPE%// END_TIME_LINE_PROTOTYPE%1
%$$

$head Purpose$$
The $code time_line_vec$$ class is used to create a vector of
$cref/time lines/numeric_average/Time Line, G/$$
for numeric approximation of averages with respect to age and time.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float>
bool time_line_vec<Float>::near_equal(double x, double y)
{	// some constants for near equal calculations
	double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
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

// BEGIN_CONSTRUCTOR_PROTOTYPE
template <class Float>
time_line_vec<Float>::time_line_vec(
	const CppAD::vector<double>& age_grid
)
// END_CONSTRUCTOR_PROTOTYPE
:
age_grid_(age_grid_)
{
# ifndef NDEBUG
	assert( 2 <= age_grid_.size() );
	for(size_t j = 1; j < age_grid.size(); ++j)
		assert( age_grid_[j-1] < age_grid_[j] );
# endif
}

// BEGIN_INITIALIZE_PROTOTYPE
template <class Float>
void time_line_vec<Float>::initialize(
	const double& age_lower  ,
	const double& age_upper  )
// END_INITIALIZE_PROTOTYPE
{	assert( age_lower <= age_upper );
	assert( age_grid_[0] <= age_lower );
	assert( age_upper <= age_grid_[age_grid_.size() - 1] );
	// -----------------------------------------------------------------
	// vec_age_
	vec_age_.resize(0);
	//
	// skip age grid values that are less than age_lower
	size_t age_index = 0;
	while( age_grid_[age_index] < age_lower )
		++age_index;
	//
	// check if age_lower is in age_grid
	bool found = near_equal( age_lower, age_grid_[age_index] );
	if( not found )
	{	assert( age_lower < age_grid_[age_index] );
		vec_age_.push_back( age_lower );
	}
	//
	// include age_grid values in [ age_lower, age_upper ]
	while( age_grid_[age_index] < age_upper )
	{	vec_age_.push_back( age_grid_[age_index] );
		++age_index;
	}
	//
	// check if age_upper is in age_grid
	found = near_equal( age_upper, age_grid_[age_index] );
	if( not found )
	{	assert( age_upper < age_grid_[age_index] );
		vec_age_.push_back( age_upper );
	}
	// -----------------------------------------------------------------
	// vec_
	size_t n_age = vec_age_.size();
	vec_.resize(n_age);
	for(size_t j = 0; j < n_age; ++j)
		vec_[j].resize(0);
}

// BEGIN_VEC_AGE_PROTOTYPE
template <class Float>
const CppAD::vector<double> time_line_vec<Float>::vec_age(void) const
// END_VEC_AGE_PROTOTYPE
{	return vec_age_; }


// BEGIN_ADD_POINT_PROTOTYPE
template <class Float>
void time_line_vec<Float>::add_point(
	const size_t&     age_index ,
	const time_point& point     )
// END_ADD_POINT_PROTOTYPE
{	// this time line
	CppAD::vector<time_point>& time_line = vec_[age_index];
	size_t n_time = time_line.size();
	//
	// time index at which to insert this point
	size_t time_index = 0;
	while( time_index < n_time && time_line[time_index].time < point.time )
		++time_index;
	//
	// case where this point goes at the end of the line
	if( time_index == n_time )
	{	time_line.push_back(point);
		return;
	}
	//
	// case where this point is inserted at time_index
	time_line.push_back( time_line[n_time - 1] );
	for(size_t i = n_time - 1; i >= time_index; --i)
		time_line[i+1] = time_line[i];
	time_line[time_index] = point;
	//
	return;
}

// BEGIN_TIME_LINE_PROTOTYPE
template <class Float>
const CppAD::vector<typename time_line_vec<Float>::time_point>&
time_line_vec<Float>::time_line(const size_t& age_index) const
// END_TIME_LINE_PROTOTYPE
{	return vec_[age_index]; }

} // END_DISMOD_AT_NAMESPACE
