// Child table for section set_command
document.write('\
<select onchange="set_command_child(this)">\
<option>set_command-&gt;</option>\
<option>set_command.py</option>\
</select>\
');
function set_command_child(item)
{	var child_list = [
		'set_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
