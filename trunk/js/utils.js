define	( []
		, function() {
var utils = {
	XHR : function(method, ad, params) {
		// method	: GET or POST
		// ad		: adress of the ressource to be loaded
		// params : An object containing two possible parameters.
		//		- onload : a function taking no argument, response will be contains in object this.
		//		- variables : an object containing a set of attribute<->value
		var xhr = new XMLHttpRequest();
		xhr.onload = params.onload || null;
		xhr.open(method, ad);
		if(method == 'POST') {xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');}
		var variables   = params.variables || null
		  , str			= '';
		for(var i in variables) {
			 str += i + '=' + encodeURIComponent( variables[i] ) + '&';
			}
		xhr.send( str );
	}
};

return utils;
});