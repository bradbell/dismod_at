// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef IPOPT_FIXED_NLP_HPP
# define IPOPT_FIXED_NLP_HPP

/*
$begin ipopt_fixed$$
$spell
	Ipopt
$$

$section Ipopt NLP Class Used to Optimize Fixed Effects$$

$childtable%devel/approx_mixed/ipopt_fixed.cpp%$$

$end
-----------------------------------------------------------------------------
*/
# include <coin/IpTNLP.hpp>
# include <dismod_at/approx_mixed.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
	//
	// ipopt_fixed
	class ipopt_fixed : public Ipopt::TNLP
	{
	private:
		// approx_mixed types used by this class
		typedef approx_mixed::d_vector     d_vector;
		//
		// Ipopt types used by this class
		typedef Ipopt::Number               Number;
		typedef Ipopt::Index                Index;
		typedef Ipopt::TNLP::IndexStyleEnum IndexStyleEnum;
		//
		// private member variables set during constructor
		const size_t n_fixed_;            // number of fixed effects
		const size_t n_random_;           // number of random effects
		//
		const d_vector& fixed_lower_;     // fixed effects lower limits
		const d_vector& fixed_in_;        // fixed effects initial value
		const d_vector& fixed_upper_;     // fixed effects upper limit
		const d_vector& random_in_;       // random effects initial value
		//
		// set during constructor, may be modified by self when evaluating.
		approx_mixed&   approx_object_;   // approx_mixed for this problem
		//
		// set during constructor, otherwise const
		size_t prior_n_abs_;      // number of absolute values in prior
		//
	public:
		//
		// did finalize_solution agree that the solution had converged
		bool finalize_solution_ok_;
		//
		// default constructor
		ipopt_fixed(
			const d_vector& fixed_lower   ,
			const d_vector& fixed_in      ,
			const d_vector& fixed_upper   ,
			const d_vector& random_in     ,
			approx_mixed&   approx_object
		);
		//
		// default destructor
		virtual ~ipopt_fixed(void);
		//
		virtual bool get_nlp_info(
			Index&          n            ,
			Index&          m            ,
			Index&          nnz_jac_g    ,
			Index&          nnz_h_lag    ,
			IndexStyleEnum& index_style
		);
		virtual bool get_bounds_info(
				Index       n        ,
				Number*     x_l      ,
				Number*     x_u      ,
				Index       m        ,
				Number*     g_l      ,
				Number*     g_u
		);
		virtual bool get_starting_point(
			Index           n            ,
			bool            init_x       ,
			Number*         x            ,
			bool            init_z       ,
			Number*         z_L          ,
			Number*         z_U          ,
			Index           m            ,
			bool            init_lambda  ,
			Number*         lambda
		);
		virtual bool eval_f(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Number&         obj_value
		);
		virtual bool eval_grad_f(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Number*         grad_f
		);
		virtual bool eval_g(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Index           m        ,
			Number*         g
		);
		virtual bool eval_jac_g(
			Index           n        ,
			const Number*   x        ,
			bool            new_x    ,
			Index           m        ,
			Index           nele_jac ,
			Index*          iRow     ,
			Index*          jCol     ,
			Number*         values
		);
		virtual bool eval_h(
			Index         n              ,
			const Number* x              ,
			bool          new_x          ,
			Number        obj_factor     ,
			Index         m              ,
			const Number* lambda         ,
			bool          new_lambda     ,
			Index         nele_hess      ,
			Index*        iRow           ,
			Index*        jCol           ,
			Number*       values
		);
		virtual void finalize_solution(
			Ipopt::SolverReturn               status    ,
			Index                             n         ,
			const Number*                     x         ,
			const Number*                     z_L       ,
			const Number*                     z_U       ,
			Index                             m         ,
			const Number*                     g         ,
			const Number*                     lambda    ,
			Number                            obj_value ,
			const Ipopt::IpoptData*           ip_data   ,
			Ipopt::IpoptCalculatedQuantities* ip_cq
		);
	};
} // END_DISMOD_AT_NAMESPACE

# endif
