/**
* This file will create the necesaire links to make the documentation more interactive.
*/

//wait until the document is ready

$(document).ready(function(){

	// get each error code

	$( "#ERROR_CODE li").each(function() { 

		// get the error message

		var error_code = $( this ).text();

		var replace = "<a href='#" + error_code + "'>" + error_code + "</a>";

		//recursiveReplace('body', error_code, error_code);

		// give the element the right id

		$( this ).attr('id', error_code);

		// get the index of the element in the list

		var index_error = $( "#ERROR_CODE li" ).index( this );

		// search for the right error string and give it the right id

		$( "#ERROR_STRING li:eq(" + index_error + ")").attr('id', error_code + "_STRING");

	});
});

/*function recursiveReplace(node, search, replace) {
    if (node.nodeType == 3) { // text node
    	if($(node).text().replace(search, replace) != $(node).text()){
    		console.log($(node).text().replace(search, replace));
    	}
        $(node).html($(node).text().replace(search, replace));
    } else if (node.nodeType == 1) { // element
        $(node).contents().each(function () {
            recursiveReplace(this, search, replace);
        });
    }
}*/
/*
function recursiveReplace(node, search, replace){
	$(node+' *').each(function() {
		if($(this).get(0).nodeType == 0){
		$(this).html($(this).text().replace(search, replace));
		}
	});
}*/
function recursiveReplace(node, search, replace){
	$('body').children().each(function(){
		$(this).html( $(this).text().replace(search, replace) )}
		);
};