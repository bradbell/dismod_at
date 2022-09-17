# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
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
