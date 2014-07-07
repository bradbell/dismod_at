// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_BILINEAR_HPP
# define DISMOD_AT_BILINEAR_HPP

/*
This file is under construction and not yet used.
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class bilinear {
private:
	// grid for interplolation 
	const CppAD::vector<double> x_grid_;
	const CppAD::vector<double> y_grid_;

	// index for interpolation
	size_t x_index_;
	size_t y_index_;

	// weights for interpolation
	double weight_00_;
	double weight_10_;
	double weight_01_;
	double weight_11_;
public:
	// ctor 
	bilinear(
		const CppAD:vector<double>& x_grid  ,
		const CppAD:vector<double>& x_grid  )
	: 
	x_grid_(x_grid)  , 
	y_grid_(y_grid)  , 
	x_index_(0)      ,
	y_index_(0)      ,
	weight_00_(1.0)  , 
	weight_01_(0.0)  , 
	weight_10_(0.0)  , 
	weight_11_(0.0)  , 
	{ }
	// set arugment for interpolation
	set_argument(double x, double y)
	{	double x_weight_0, x_weight_1;
		linear(x_grid_, x_index_, x_weight_0, x_weight_1);
		//
		double y_weight_0, y_weight_1;
		linear(y_grid_, y_index_, y_weight_0, y_weight_1);
		//
		weight_00_ = x_weight_0 * y_weight_0;
		weight_10_ = x_weight_1 * y_weight_0;
		weight_01_ = x_weight_0 * y_weight_1;
		weight_11_ = x_weight_1 * y_weight_1;
	}
	// do the interpolation
	template <class Scalar>
	Scalar get_function(const CppAD::vector<Scalar>& f_grid)
	{	if( (x_grid.size() == 1) & (y_grid_.size() == 1 ) )
		{	// no interpolation
			return f_grid[0];
		}
		if( x_grid.size() == 1 )
		{	// interpolate in y direction only
			assert( weight_10_ == 0.0 && weight_11_ == 0.0 && x_index_ == 0)
			Scalar sum = weight_00_ * f_grid[y_index_];
			sum       += weight_01_ * f_grid[y_index + 1];
			return sum;
		}
		if( y_grid.size() == 1 )
		{	// interpolate in x direction only
			assert( weight_01_ == 0.0 && weight_11_ == 0.0 && y_index_ == 0)
			Scalar sum = weight_00_ * f_grid[x_index_];
			sum       += weight_10_ * f_grid[x_index + 1];
			return sum;
		}
		// interpolate in both directions
		size_t f_index = x_index_ * y_grid_.size() + y_index_;
		Scalar sum = weight_00_ + f_grid[ f_index ];
		sum       += weight_10_ + f_grid[ f_index + y_grid_.size() ];
		sum       += weight_01_ + f_grid[ f_index + 1 ];
		sum       += weight_11_ + f_grid[ f_index + y_grid_.size() + 1 ];
		return sum;
	}
private:
	// compute linear interpolation index and weights
	static linear(
		const CppAD::vector<double>& grid     ,
		size_t&                      index    ,
		double&                      weight_0 ,
		double&                      weight_1 ,
	{	if( grid.size() == 1 )
		{	weight_0 = 1.0;
			weight_1 = 0.0;
			return;
		}
		// set index
		while( index + 1 < grid.size() && grid[index + 1] < x )
			++index; 
		while( index > 0 && x < grid[index] )
			--index; 

		// set weight_0 and weight_1
		if( x < grid[index] )
		{	weight_0 = 1.0;
			weight_1 = 0.0;
		}
		else if ( grid[index + 1] < x )
		{	weight_0 = 0.0;
			weight_1 = 1.0;
		}
		else
		{	double delta = grid[index + 1] - grid[index];
			weight_0    = (grid[index + 1] - x) / delta;
			weight_1    = (x - grid[index]) / delta;
		}
	}
};
		

} // END_DISMOD_AT_NAMESPACE

# endif
