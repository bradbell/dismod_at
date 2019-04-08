// Child table for section modify_command
document.write('\
<select onchange="modify_command_child(this)">\
<option>modify_command-&gt;</option>\
<option>modify_command.py</option>\
</select>\
');
function modify_command_child(item)
{	var child_list = [
		'modify_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
