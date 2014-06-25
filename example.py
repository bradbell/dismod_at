#! /usr/bin/env python
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
import example
#
example.table.create_table()
example.table.covariate_table()
example.table.data_table()
example.table.integrand_table()
example.table.like_table()
example.table.misc_table()
example.table.multiplier_table()
example.table.node_table()
example.table.post_table()
example.table.rate_prior()
example.table.rate_table()
example.table.smooth_table()
example.table.weight_table()
#
example.get_started()
