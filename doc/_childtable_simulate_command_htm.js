// Child table for section simulate_command
document.write('\
<select onchange="simulate_command_child(this)">\
<option>simulate_command-&gt;</option>\
<option>simulate_command.py</option>\
</select>\
');
function simulate_command_child(item)
{	var child_list = [
		'simulate_command.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
