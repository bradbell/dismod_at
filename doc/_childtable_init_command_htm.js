// Child table for section init_command
document.write('\
<select onchange="init_command_child(this)">\
<option>init_command-&gt;</option>\
<option>init_command.py</option>\
</select>\
');
function init_command_child(item)
{	var child_list = [
		'init_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
