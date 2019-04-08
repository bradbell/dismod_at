// Child table for section sample_command
document.write('\
<select onchange="sample_command_child(this)">\
<option>sample_command-&gt;</option>\
<option>sample_command.py</option>\
</select>\
');
function sample_command_child(item)
{	var child_list = [
		'sample_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
