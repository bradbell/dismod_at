# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# remove template information
s/<[^<>]*>//g
s/<[^<>]*>//g
s/<[^<>]*>//g
s/<[^<>]*>//g
s/<[^<>]*>//g
# remove argument information
s/([^()]*)//g
s/([^()]*)//g
# remove names space information
s/[a-zA-Z0-9_]*:://g
s/[a-zA-Z0-9_]*:://g
s/[a-zA-Z0-9_]*:://g
s/[a-zA-Z0-9_]*:://g
