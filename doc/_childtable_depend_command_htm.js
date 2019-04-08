// Child table for section depend_command
document.write('\
<select onchange="depend_command_child(this)">\
<option>depend_command-&gt;</option>\
<option>depend_command.py</option>\
</select>\
');
function depend_command_child(item)
{	var child_list = [
		'depend_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
