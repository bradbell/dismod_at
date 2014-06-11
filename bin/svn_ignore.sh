# -------------------------------------------------------------------------- */
if [ $0 != "bin/svn_ignore.sh" ]
then
	echo "bin/svn_ignore.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
cat << EOF > bin/svn_ignore.$$
MANIFEST
dist
build
doc
doc.tgz
junk
junk.*
*.log
*.pyc
new
setup.py.old
temp
temp.sh
EOF
svn propset svn:ignore --recursive -F bin/svn_ignore.$$ .
rm bin/svn_ignore.$$
#
exit 0 
