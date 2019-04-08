// Child table for section devel
document.write('\
<select onchange="devel_child(this)">\
<option>devel-&gt;</option>\
<option>devel_table</option>\
<option>devel_utility</option>\
<option>devel_model</option>\
</select>\
');
function devel_child(item)
{	var child_list = [
		'devel_table.htm',
		'devel_utility.htm',
		'devel_model.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
