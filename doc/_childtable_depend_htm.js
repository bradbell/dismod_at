// Child table for section depend
document.write('\
<select onchange="depend_child(this)">\
<option>depend-&gt;</option>\
<option>data_depend</option>\
<option>prior_depend</option>\
</select>\
');
function depend_child(item)
{	var child_list = [
		'data_depend.htm',
		'prior_depend.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
