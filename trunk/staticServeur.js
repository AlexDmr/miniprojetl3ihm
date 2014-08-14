var RRServer = {
	  fs			: require('fs-extra')
	, express		: require('express')
	, bodyParser	: require("body-parser")
	, DOMParser		: require('xmldom').DOMParser
	, XMLSerializer	: require('xmldom').XMLSerializer
	, app			: null
	, nurses		: []
	, init			: function(port) {
		 var self = this;
		 
		// XML document
		 this.domParser		= new this.DOMParser();
		 this.xmlSerializer	= new this.XMLSerializer();
		 this.fs.readFile(__dirname + '/data/cabinetInfirmier.xml'
						 , function(err, dataObj) {
							 if(err) {
								 console.error("Problem reading file /data/cabinetInfirmier.xml", err);
								} else {try {console.log("typeof data :", typeof data );
											 var data = new String();
											 data = data.concat(dataObj);
											 self.doc = self.domParser.parseFromString(data, 'text/xml');
											 console.log("/data/cabinetInfirmier.xml successfully parsed !");
											} catch(err2) {console.error('Problem parsing /data/cabinetInfirmier.xml', err2);}
									   }
							}
						 );
		 
		// HTTP server
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
		 this.app.post('/addPatient', function(req, res) {
							 console.log("/addPatient", req.body);
							 var patients = self.doc.getElementsByTagName('patients')[0]
							 var newPatient = self.doc.createElement('patient');
								patients.appendChild( newPatient );
								// Name
								var nom = self.doc.createElement('nom');
								nom.appendChild( self.doc.createTextNode(req.body.patientName) );
								newPatient.appendChild( nom );
								// Forname
								var prénom = self.doc.createElement('prénom');
								prénom.appendChild( self.doc.createTextNode(req.body.patientForname) );
								newPatient.appendChild( prénom );
								// Social security number
								var numéro = self.doc.createElement('numéro');
								numéro.appendChild( self.doc.createTextNode(req.body.patientNumber) );
								newPatient.appendChild( numéro );
								// Sex
								var sexe = self.doc.createElement('sexe');
								sexe.appendChild( self.doc.createTextNode(req.body.patientSex) );
								newPatient.appendChild( sexe );
								// Birthday
								var naissance = self.doc.createElement('naissance');
								naissance.appendChild( self.doc.createTextNode(req.body.patientBirthday) );
								newPatient.appendChild( naissance );
								// Adress
								var adresse = self.doc.createElement('adresse');
								newPatient.appendChild( adresse );
									var étage = self.doc.createElement('étage');
									adresse.appendChild( étage );
									var numAdress = self.doc.createElement('numéro');
									adresse.appendChild( numAdress );
									var rue = self.doc.createElement('rue');
									adresse.appendChild( rue );
									var ville = self.doc.createElement('ville');
									adresse.appendChild( ville );
									// <étage>4</étage>
									// <numéro>46</numéro>
									// <rue>avenue Félix Viallet</rue>
									// <ville>Grenoble</ville>
									// <codePostal
							 console.log( self.xmlSerializer.serializeToString(newPatient) );
							 res.writeHead(200);
							 res.end();
							}
				);
		 this.app.post('/INFIRMIERE', function(req, res) {
							 DistributionState.fs.readFile(__dirname + '/infirmiere',
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
