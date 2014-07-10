var RRServer = {
	  fs		: require('fs-extra')
	, express	: require('express')
	, bodyParser: require("body-parser")
	, app		: null
	, nurses	: []
	, init		: function(port) {
		 this.app	  = this.express();
		 this.server  = this.app.use(this.express.static(__dirname))
								.use( this.bodyParser.urlencoded({ extended: false }) )
								.listen(port) ;
		 this.app.post('/', function(req, res) {
				 // POST contains a login that identify the secretary or one nurse
				 // Depending on the login, transmit the right webpage
				 switch(req.body.login) {
					 case 'Secretaire':
						RRServer.fs.readFile(__dirname + '/secretary.html',
							  function (err, data) {
								if (err) {
									 res.writeHead(500);
									 return res.end('Error loading secretary.html : ', err);
									}
								res.writeHead(200);
								res.write( data.toString() );
								res.end();
							  });
					 break;
					 default: // Is it a nurse ?
					 res.writeHead(200);
					 res.write("Unknown login : " + req.body.login);
					 res.end();
					}
				});
		 this.app.get('/infirmiere', function(req, res) {
							 DistributionState.fs.readFile(__dirname + '/XXX',
								  function (err, data) {
									if (err) {
										 res.writeHead(500);
										 return res.end('Error loading infirmiere');
										}
									res.writeHead(200);
									res.write( data.toString() );
									res.end();
								  });
							}
				);

		}
};

var params = {}, p;
for(var i=2; i<process.argv.length; i++) {
	p = process.argv[i].split(':');
	params[p[0]] = p[1];
}

var port = params.port || 8080;
console.log("Usage :\n\tnode staticServeur.js [port:PORT]\n\tDefault port is 8080.");
console.log("HTTP server listening on port " + port);
RRServer.init( port );
