var RRServer = {
	  fs			: require('fs-extra')
	, express		: require('express')
	, bodyParser	: require("body-parser")
	, DOMParser		: require('xmldom').DOMParser
	, XMLSerializer	: require('xmldom').XMLSerializer
	, multer		: require('multer')
	, app			: null
	, nurses		: []
	, saveXML		: function(res) {
		 var self = this;
		 self.fs.writeFile	( './data/cabinetInfirmier.xml' // path
							, self.xmlSerializer.serializeToString( self.doc ) // data
							, function(err) { // callback
								 if (err) {
									 console.error( "Error writing ./data/cabinetInfirmier.xml:\n", err);
									 res.writeHead(500);
									 res.write("Error writing ./data/cabinetInfirmier.xml:\n", err);
									} else {res.writeHead(200);}
								 res.end();
								}
							);
		}
	, getPatient	: function(number) {
		 var L = this.doc.getElementsByTagName('patient')
		   , num_node;
		 for(var i=0; i<L.length; i++) {
			 num_node = L[i].getElementsByTagName('numéro')[0];
			 if(num_node.textContent === number) {
				 return L[i];
				}
			}
		 return null;
		}
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
		 this.server  = this.app.use( this.express.static(__dirname) )
								.use( this.bodyParser.urlencoded({ extended: false }) )
								.use( this.bodyParser.json() )
								.use( this.multer({ dest: './uploads/'}) )
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
					 res.write("Unsupported login : " + req.body.login);
					 res.end();
					}
				});
		 this.app.post('/addPatient', function(req, res) {
							 console.log("/addPatient, \nreq.body:\n\t", req.body, "\n_______________________");
							 req.body.patientName		= req.body.patientName		|| '';
							 req.body.patientForname	= req.body.patientForname	|| '';
							 req.body.patientNumber		= req.body.patientNumber	|| '';
							 req.body.patientSex		= req.body.patientSex		|| '';
							 req.body.patientBirthday	= req.body.patientBirthday	|| '';
							 req.body.patientFloor		= req.body.patientFloor		|| '';
							 req.body.patientStreet		= req.body.patientStreet	|| '';
							 req.body.patientPostalCode	= req.body.patientPostalCode|| '';
							 req.body.patientCity		= req.body.patientCity		|| '';
							 
							 var patients = self.doc.getElementsByTagName('patients')[0]
							 // Is it a new patient or not ?
							 var newPatient = self.getPatient( req.body.patientNumber );
							 if(newPatient === null) {
								 newPatient = self.doc.createElement('patient');
								 patients.appendChild( newPatient );
								} else	{// Erase subtree
										 while(newPatient.childNodes.length) {
											 newPatient.removeChild( newPatient.childNodes[0] );
											}
										}
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
								// Visites
								var visite = self.doc.createElement('visite');
								visite.setAttribute('date', "2014-12-08");
								newPatient.appendChild( visite );
								// Adress
								var adresse = self.doc.createElement('adresse');
								newPatient.appendChild( adresse );
									var étage = self.doc.createElement('étage');
									étage.appendChild( self.doc.createTextNode(req.body.patientFloor) );
									adresse.appendChild( étage );
									var numAdress = self.doc.createElement('numéro');
									numAdress.appendChild( self.doc.createTextNode(req.body.patientFloor) );
									adresse.appendChild( numAdress );
									var rue = self.doc.createElement('rue');
									rue.appendChild( self.doc.createTextNode(req.body.patientStreet) );
									adresse.appendChild( rue );
									var ville = self.doc.createElement('ville');
									ville.appendChild( self.doc.createTextNode(req.body.patientCity) );
									adresse.appendChild( ville );
									var codePostal = self.doc.createElement('codePostal');
									codePostal.appendChild( self.doc.createTextNode(req.body.patientPostalCode) );
									adresse.appendChild( codePostal );
									
							 console.log( self.xmlSerializer.serializeToString(newPatient) );
							 self.saveXML(res);
							}
				);
		 this.app.post( '/affectation'
					  , function(req, res) {
							 if( typeof req.body.infirmier	=== 'undefined'
							   ||typeof req.body.patient	=== 'undefined' ) {
							    res.writeHead(500);
								res.end("You should specify 'infirmier' with her id and 'patient' with her social security number in your request.");
							   } else {// Get node corresponding to the nurse
									   var nurse = self.doc.getElementById( req.body.infirmier );
									   if (nurse || req.body.infirmier === 'none') {
											// Get node corresponding to the patient
											var LP = self.doc.getElementsByTagName('patient')
											  , node_num;
											for(var i=0; i<LP.length; i++) {
												 node_num = LP[i].getElementsByTagName('numéro')[0]; //querySelector('numéro');
												 if( node_num.textContent === req.body.patient ) {
													 // Do stuff here
													 // LP[i].querySelector('visite').setAttribute('intervenant', req.body.infirmier);
													 if( req.body.infirmier === 'none' ) {req.body.infirmier = '';}
													 LP[i].getElementsByTagName('visite')[0].setAttribute('intervenant', req.body.infirmier);
													 self.saveXML(res);
													 break;
													}
												}
										} else {res.writeHead(500);
												res.end("There is no nurse identified by id", req.body.infirmier);
												}
									  }
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
