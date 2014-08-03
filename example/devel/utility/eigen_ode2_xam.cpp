// $Id$
/* --------------------------------------------------------------------------
dismod_ode: MCMC Estimation of Disease Rates as Functions of Age
          Copyright (C) 2013 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin eigen_ode2_xam.cpp$$
$spell
	eigen
$$

$section Example and Test of Eigenvector Method of Solving ODE$$
$index test, eigen_ode2$$
$index example, eigen_ode2$$

$code
$verbatim%example/devel/utility/eigen_ode2_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$
$end
*/
// BEGIN C++
# include <dismod_at/dismod_at.hpp>
	
namespace {
	typedef CppAD::AD<double> Float;
	class Fun {
		typedef CppAD::vector<Float> vector;
	private:
		vector b_;
	public:
		void Ode(const Float& t, const vector& y, vector& yp)
		{	yp[0] = b_[0] * y[0] + b_[1] * y[1]; 
			yp[1] = b_[2] * y[0] + b_[3] * y[1]; 
			return;
		}
		void set(const vector& b)
		{	b_.resize( b.size() );
			b_ = b;
		}
	};
}

bool eigen_ode2_xam(void)
{	bool ok = true;
	typedef CppAD::vector<Float> vector;
	//
	// solve ODE using eigen_ode2
	Float tf = 0.25;
	vector b(4), yi(2), yf(2);
	b[0] = -3.0;  b[1] = 0.0;
	b[2] = 0.0;   b[3] = -4.0;
	yi[0] = 1.0;
	yi[1] = 2.0;
	dismod_at::eigen_ode2(tf, b, yi, yf);
	//
	// solve ODE using Runge45
	Fun F;
	F.set(b);
	size_t M  = 20;
	Float ti = 0.0;
	vector xi = yi;
	vector xf = CppAD::Runge45(F, M, ti, tf, yi);
	//
	// splitting case:
	// b[0] = -3.0 , b[1] =  0.0
	// b[2] =  0.0 , b[3] = -4.0
	ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
	ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
	//
	// switch during splitting case:
	b[0] = -3.0 , b[1] =  1.0;
	b[2] =  0.0 , b[3] = -4.0;
	F.set(b);
	dismod_at::eigen_ode2(tf, b, yi, yf);
	xf = CppAD::Runge45(F, M, ti, tf, yi);
	ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
	ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
	//
	// eigen vector case 
	b[0] = -3.0 , b[1] =  0.5;
	b[2] =  1.0 , b[3] = -4.0;
	F.set(b);
	dismod_at::eigen_ode2(tf, b, yi, yf);
	xf = CppAD::Runge45(F, M, ti, tf, yi);
	ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
	ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
	//
	return ok;
}
// END C++
