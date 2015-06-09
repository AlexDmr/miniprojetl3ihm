/**_________________________________________________________________________________________________________________________________ 
  * Get external libraries ---------------------------------------------------------------------------------------------------------
**/
var fs				= require('fs-extra')				// Access files
  , express			= require('express')				// Framework to implement HTTP server
  , bodyParser		= require("body-parser")			// plugin for parsing HTTP requests
  , DOMParser		= require('xmldom').DOMParser		// DOM parser 	  (string -> DOM)
  , XMLSerializer	= require('xmldom').XMLSerializer	// DOM serializer (DOM -> string)
  , multer			= require('multer')					// plugin for transmiting file via HTTP
  , request			= require('request')				// send HTTP queries
  , xmlSerializer	= null
  , domParser		= null
  ;

  /**_________________________________________________________________________________________________________________________________ 
  * Save the XML into a file, file acces is asynchronous -----------------------------------------------------------------------------
  *   - doc : the document containing the XML ----------------------------------------------------------------------------------------
  *   - res : the result stream of a client HTTP request -----------------------------------------------------------------------------
**/
function saveXML(doc, res) {
	fs.writeFile( './data/cabinetInfirmier.xml'
				, xmlSerializer.serializeToString( doc )
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

/**_________________________________________________________________________________________________________________________________ 
  * Returns DOM node of patient identified by numlber in document doc or null if there is no such patient --------------------------
**/
function getPatient(doc, number) {
	var L = doc.getElementsByTagName('patient')
	  , num_node;
	for(var i=0; i<L.length; i++) {
		 num_node = L[i].getElementsByTagName('numéro')[0];
		 if(num_node.textContent === number) {return L[i];}
		}
	return null;
}


/**_________________________________________________________________________________________________________________________________ 
  * Define HTTP server, implement some ressources ----------------------------------------------------------------------------------
  *   - port : the TCP port on which the HTTP server will be listening -------------------------------------------------------------
**/
function init(port, applicationServerIP, applicationServerPort) {
	domParser		= new DOMParser()				// an instance of DOM parser     (string -> DOM)
	xmlSerializer	= new XMLSerializer()			// an instance of DOM serializer (DOM -> string)
	var doc											// will reference the document representing the XML structure
	  , app											// will reference the HTTP server
	  , applicationServer = {ip: applicationServerIP, port: applicationServerPort}	// Application server IP and port that is in charge of optimizing nurses' travels, by default, this server
	  ;
	
	// Read and parse the XML file containing the data
	fs.readFile	(__dirname + '/data/cabinetInfirmier.xml'
				, function(err, dataObj) {
					 if(err) {
						 console.error("Problem reading file /data/cabinetInfirmier.xml", err);
						} else {try {console.log("typeof dataObj :", typeof dataObj );
									 var data = ""; //new String();
									 data = data.concat(dataObj);
									 doc  = domParser.parseFromString(data, 'text/xml');
									 console.log("/data/cabinetInfirmier.xml successfully parsed !");
									} catch(err2) {console.error('Problem parsing /data/cabinetInfirmier.xml', err2);}
							   }
					}
				);
				
	// Initialize the HTTP server
	app	= express();
	app .use( express.static(__dirname) )						// Associate ressources for accessing local files
		.use( bodyParser.urlencoded({ extended: false }) )		// Add a parser for urlencoded HTTP requests
		.use( bodyParser.json() )								// Add a parser for json HTTP request
		.use( multer({ dest: './uploads/'}) )					// Add a parser for file transmission
		.listen(port) ;											// HTTP server listen to this TCP port
	
	// Define HTTP ressource GET /
	app.get	( '/'
			, function(req, res) {											// req contains the HTTP request, res is the response stream
				 // console.log('Getting /');
				 fs.readFile( __dirname + '/start.html'
							, function (err, data) {
									if (err) {
										 res.writeHead(500);
										 return res.end('Error loading start.html : ' + err);
										}
									// Parse it so that we can add secretary and all nurses
									var doc = domParser.parseFromString( data.toString() );
									var datalist = doc.getElementById('logins');
									var L_nurses = doc.getElementsByTagName('infirmier');
									for(var i=0; i<L_nurses.length; i++) {
										 var option = doc.createElement('option');
										 option.setAttribute( 'value', L_nurses[i].getAttribute('id') );
										 option.textContent = L_nurses[i].getElementsByTagName('prénom')[0].textContent
															  + ' '
															  + L_nurses[i].getElementsByTagName('nom')[0].textContent
															;
										 datalist.appendChild(option);
										}
									res.writeHead(200);
									res.write( xmlSerializer.serializeToString(doc) );
									res.end();
								  });
				}
			);
	
	// Define HTTP ressource POST /, contains a login that identify the secretary or one nurse
	app.post( '/'
			, function(req, res) {
				 switch(req.body.login) {
					 case 'Secretaire':
						fs.readFile( __dirname + '/secretary.html',
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
				}
			);
			
	// Define HTTP ressource PORT /addPatient, may contains new patient information
	app.post( '/addPatient'
			, function(req, res) {
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
				 
				 var patients = doc.getElementsByTagName('patients')[0];
				 // Is it a new patient or not ?
				 var newPatient = getPatient(doc, req.body.patientNumber);
				 if(newPatient === null) {
					 newPatient = doc.createElement('patient');
					 patients.appendChild( newPatient );
					} else	{// Erase subtree
							 while(newPatient.childNodes.length) {
								 newPatient.removeChild( newPatient.childNodes[0] );
								}
							}
					// Name
					var nom = doc.createElement('nom');
					nom.appendChild( doc.createTextNode(req.body.patientName) );
					newPatient.appendChild( nom );
					// Forname
					var prénom = doc.createElement('prénom');
					prénom.appendChild( doc.createTextNode(req.body.patientForname) );
					newPatient.appendChild( prénom );
					// Social security number
					var numéro = doc.createElement('numéro');
					numéro.appendChild( doc.createTextNode(req.body.patientNumber) );
					newPatient.appendChild( numéro );
					// Sex
					var sexe = doc.createElement('sexe');
					sexe.appendChild( doc.createTextNode(req.body.patientSex) );
					newPatient.appendChild( sexe );
					// Birthday
					var naissance = doc.createElement('naissance');
					naissance.appendChild( doc.createTextNode(req.body.patientBirthday) );
					newPatient.appendChild( naissance );
					// Visites
					var visite = doc.createElement('visite');
					visite.setAttribute('date', "2014-12-08");
					newPatient.appendChild( visite );
					// Adress
					var adresse = doc.createElement('adresse');
					newPatient.appendChild( adresse );
						var étage = doc.createElement('étage');
						étage.appendChild( doc.createTextNode(req.body.patientFloor) );
						adresse.appendChild( étage );
						var numAdress = doc.createElement('numéro');
						numAdress.appendChild( doc.createTextNode(req.body.patientFloor) );
						adresse.appendChild( numAdress );
						var rue = doc.createElement('rue');
						rue.appendChild( doc.createTextNode(req.body.patientStreet) );
						adresse.appendChild( rue );
						var ville = doc.createElement('ville');
						ville.appendChild( doc.createTextNode(req.body.patientCity) );
						adresse.appendChild( ville );
						var codePostal = doc.createElement('codePostal');
						codePostal.appendChild( doc.createTextNode(req.body.patientPostalCode) );
						adresse.appendChild( codePostal );
						
				 console.log( xmlSerializer.serializeToString(newPatient) );
				 saveXML(doc, res);
				}
			);

	// Define HTTP ressource POST /affectation, associate a patient with a nurse
	app.post( '/affectation'
			, function(req, res) {
					if( typeof req.body.infirmier	=== 'undefined'
					  ||typeof req.body.patient		=== 'undefined' ) {
							res.writeHead(500);
							res.end("You should specify 'infirmier' with her id and 'patient' with her social security number in your request.");
						   } else {// Get node corresponding to the nurse
								   var nurse = doc.getElementById( req.body.infirmier );
								   if (nurse || req.body.infirmier === 'none') {
										// Get node corresponding to the patient
										var LP = doc.getElementsByTagName('patient')
										  , node_num;
										for(var i=0; i<LP.length; i++) {
											 node_num = LP[i].getElementsByTagName('numéro')[0];
											 if( node_num.textContent === req.body.patient ) {
												 if( req.body.infirmier === 'none' ) {req.body.infirmier = '';}
												 LP[i].getElementsByTagName('visite')[0].setAttribute('intervenant', req.body.infirmier);
												 saveXML(doc, res);
												 break;
												}
											}
									} else {res.writeHead(500);
											res.end("There is no nurse identified by id", req.body.infirmier);
											}
								  }
				}
			);

	// Define HTTP ressource POST /INFIRMIERE
	app.post( '/configureOptimizationServer'
			, function(req, res) {
				 var IP	  = req.body.IP
				   , port = req.body.port;
				 
				}
			);
	app.post( '/INFIRMIERE'
			, function(req, res) {
				 res.end("INFIRMIERE " + req.body.id + ". WARNING: You should configure the optimization application server IP and port. By default, the optimization application server is configured to be the HCI one.");
				}
			);
	app.post( '/infirmiere'
			, function(req, res) {
				 // res.writeHead(200);
				 // res.write( self.xmlSerializer.serializeToString(doc) );
				 // res.end();
				 request.post( { url	: 'http://' + applicationServer.IP + ':' + applicationServer.port + '/INFIRMIERE'
							   , form	: req.body
							   }
							 , function(err, httpResponse, body) {
									 if(err) {
										 res.writeHead(400);
										 res.write("Error on the optimization application server: ");
										 res.end( err );
										} else {res.end( body );
											   }
									}
							 );
				}
			);
}


/**_________________________________________________________________________________________________________________________________ 
  * Parse command line parameters and initialize everything ------------------------------------------------------------------------
**/
var params = {}, p;
for(var i=2; i<process.argv.length; i++) {
	p = process.argv[i].split(':');
	params[p[0]] = p[1];
}

var port					= params.port       || 8080
  , applicationServerIP		= params.remoteIP   || '127.0.0.1'
  , applicationServerPort	= params.remotePort || 8080
  ;
console.log("Usage :\n\tnode staticServeur.js [port:PORT]\n\tDefault port is 8080.");
console.log("HTTP server listening on port " + port);
init(port, applicationServerIP, applicationServerPort);
