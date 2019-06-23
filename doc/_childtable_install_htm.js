// Child table for section install
document.write('\
<select onchange="install_child(this)">\
<option>install-&gt;</option>\
<option>dock_dismod_at.sh</option>\
<option>install_unix</option>\
</select>\
');
function install_child(item)
{	var child_list = [
		'dock_dismod_at.sh.htm',
		'install_unix.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
