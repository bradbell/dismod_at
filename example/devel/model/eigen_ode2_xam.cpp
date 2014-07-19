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
$verbatim%example/devel/model/eigen_ode2_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$
$end
*/
// BEGIN C++
# include <cmath>

// Runge45 requires fabs to be defined (not std::fabs)
// <cppad/cppad.hpp> defines this for doubles, but runge_45.hpp does not. 
using std::fabs;
# include <cppad/runge_45.hpp>

# include <dismod_at/dismod_at.hpp>
	
namespace {
	class Fun {
		typedef CppAD::vector<double> vector;
	private:
		vector a_;
	public:
		void Ode(const double& t, const vector& y, vector& yp)
		{	yp[0] = a_[0] * y[0] + a_[1] * y[1]; 
			yp[1] = a_[2] * y[0] + a_[3] * y[1]; 
			return;
		}
		void set(const vector& a)
		{	a_.resize( a.size() );
			a_ = a;
		}
	};
}

bool eigen_ode2_xam(void)
{	bool ok = true;
	typedef CppAD::vector<double> vector;
	//
	// solve ODE using eigen_ode2
	double tf = 0.25;
	vector a(4), yi(2), yf(2);
	a[0] = -3.0;  a[1] = 0.0;
	a[2] = 0.0;   a[3] = -4.0;
	yi[0] = 1.0;
	yi[1] = 2.0;
	dismod_at::eigen_ode2(tf, a, yi, yf);
	//
	// solve ODE using Runge45
	Fun F;
	F.set(a);
	size_t M  = 20;
	double ti = 0.0;
	vector xi = yi;
	vector xf = CppAD::Runge45(F, M, ti, tf, yi);
	//
	// splitting case:
	// a[0] = -3.0 , a[1] =  0.0
	// a[2] =  0.0 , a[3] = -4.0
	ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
	ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
	//
	// switch during splitting case:
	a[0] = -3.0 , a[1] =  1.0;
	a[2] =  0.0 , a[3] = -4.0;
	F.set(a);
	dismod_at::eigen_ode2(tf, a, yi, yf);
	xf = CppAD::Runge45(F, M, ti, tf, yi);
	ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
	ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
	//
	// eigen vector case 
	a[0] = -3.0 , a[1] =  0.5;
	a[2] =  1.0 , a[3] = -4.0;
	F.set(a);
	dismod_at::eigen_ode2(tf, a, yi, yf);
	xf = CppAD::Runge45(F, M, ti, tf, yi);
	ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
	ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
	//
	return ok;
}
// END C++
