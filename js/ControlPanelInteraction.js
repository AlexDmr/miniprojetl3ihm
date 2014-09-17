define	( [ 'L3_List'
		  , 'MarkerWithLabel'
		  , 'utils'
		  ]
		, function(L3_List, MarkerWithLabel, utils) {
var ControlPanelInteraction = {
	  map					: null
	, L_markers				: []
	, init					: function(mapId, tableInfirmiereId, newPatientMapId) {
		 var self = this;
		 
		 // Drag & Drop
		 this.draggingPatientCard = null;
		 
		 // Remaining patients
		 this.div_remainingPatient		= document.getElementById('PatientsRestants');
		 
		 // Geocoder
		 this.geocoder = new google.maps.Geocoder();

		// New patient
		 var div_addPatient = document.getElementById('addPatient');
		 var btAddPatient	= document.getElementById('btAddPatient');
		 var bt_add			= this.div_remainingPatient.querySelector('button');
		 btAddPatient.addEventListener( 'click'
								, function() {
									 // Send Ajax request and wait for response to close the add window
									 // But unactivate it during the AJAX call...
									 utils.XHR	( 'POST', '/addPatient'
												, { onload	: function() {
																 div_addPatient.classList.remove('display');
																 console.log('addPatient response :', this);
																}
												  , form	: div_addPatient.querySelector('form')
												  }
												);
									 
									}
								, false );
		 bt_add.addEventListener( 'click'
								, function() {div_addPatient.classList.add('display');}
								, false );
		 this.newPatientMap = new google.maps.Map	( document.getElementById(newPatientMapId)
										, { center: new google.maps.LatLng(45.193861, 5.768843)
										  , zoom: 11
										  }
										);
		 this.newPatientMarker = new google.maps.Marker(
				{ position	: new google.maps.LatLng(0, 0)
				, map		: this.newPatientMap
				, title		: "Hello World!"
				} );
		 google.maps.event.addListener(
				  this.newPatientMap
				, 'click'
				, function(evt) {
					 // console.log("Click on new patient map", self.newPatientMarker.position, evt.latLng);
					 self.newPatientMarker.setPosition(evt.latLng);
					 self.geocoder.geocode( {'latLng': evt.latLng}
									 , function(results, status) {
											if (status == google.maps.GeocoderStatus.OK) {
												 console.log(results);
												 var res = results[0].address_components;
												 // Create a table associating response type to their long name
												 var adComponents = {}
												 for(var i=0; i<res.length; i++) {
													 adComponents[ res[i].types[0] ] = res[i].long_name;
													}
												 console.log( adComponents );
												 document.getElementById('patientStreet').value = (adComponents.street_number?(adComponents.street_number+' '):'') + adComponents.route;
												 document.getElementById('patientPostalCode').value = adComponents.postal_code;
												 document.getElementById('patientCity').value = adComponents.locality;
												} else {console.error("Error in geocoding ", evt.latLng, " : ", status);}
											}
									 );
					}
				);
		 
		// All patients
		 this.map = new google.maps.Map	( document.getElementById(mapId)
										, { center: new google.maps.LatLng(45.193861, 5.768843)
										  , zoom: 11
										  }
										);
		 // google.maps.event.addListener(this.map, 'click', function(e) {self.addMark(e);});
		 this.serviceDirection	= new google.maps.DirectionsService();
		 this.directionsDisplay = new google.maps.DirectionsRenderer({ draggable		: false
																	 , suppressMarkers	: true
																	 , preserveViewport	: true 
																	 });
		 this.directionsDisplay.setMap( this.map );
		 
		 // Access to the server data
		 this.tableInfirmiere     = document.getElementById( tableInfirmiereId );
		 this.bodytableInfirmiere = this.tableInfirmiere.querySelector('tbody');
		 utils.XHR( 'GET', '/data/cabinetInfirmier.xml'
				  , {onload : function() {
								 console.log("GET cabinet :", this);
								// Get references to relevant elements
								 var tbody = document.querySelector('#Affectations > tbody');
								 var xml = this.responseXML;
								 self.dataXML = xml;
								 
								// Make a drop zone of the remaining patients
								 self.MakeDropZoneOf( self.div_remainingPatient, 'none' );
								 
								// For each nurse, get the list of patients
								// "unallocated" patient are listed appart
								 var infirmieres = {none: {html: null, visites:[]}};
								 var L_nodes_infirmiers = xml.querySelectorAll("cabinet > infirmiers > infirmier");
								 for(var i=0; i<L_nodes_infirmiers.length; i++) {
									 var infirmiere = infirmieres[ L_nodes_infirmiers[i].getAttribute('id') ] = {node: L_nodes_infirmiers[i], visites:[]}
									 var html_tr = document.createElement('tr');
									 tbody.appendChild( html_tr );
									 infirmiere.html = html_tr;
									}
								 var L_node_patients = xml.querySelectorAll("cabinet > patients > patient");
								 for(var i=0; i<L_node_patients.length; i++) {
									 var intervenant = L_node_patients[i].querySelector('visite').getAttribute('intervenant');
									 console.log('intervenant :', intervenant);
									 var infirmier = intervenant?String(intervenant):'none';
									 infirmieres[ infirmier ].visites.push( L_node_patients[i] );
									}
								// Display nurses and related patients on the table
								 for(var i in infirmieres) {
									 if(i === 'none') continue;
									 var infirmiere = infirmieres[i];
									 console.log(i, ' : ', infirmiere, ' in ', infirmieres);
									 var tr			= infirmiere.html;
									 var td			= document.createElement('td');
									 td.innerHTML	= infirmiere.node.querySelector('prénom').textContent
													+ ' '
													+ infirmiere.node.querySelector('nom').textContent;
									 tr.appendChild(td);
									 td	= document.createElement('td');
										// Act as a drop zone for patientCard
										self.MakeDropZoneOf(td, i);
									 tr.appendChild(td);
									 for(var v=0; v<infirmiere.visites.length; v++) {
										 td.appendChild( self.addPatientUI( infirmiere.visites[v] ) );
										}
									}
								// Display remaining patients
								 var infirmiere = infirmieres['none'];
								 if(infirmiere.visites.length) {self.div_remainingPatient.classList.add('display');}
								 for(var v=0; v<infirmiere.visites.length; v++) {
									 self.div_remainingPatient.appendChild( self.addPatientUI( infirmiere.visites[v] ) );
									}
								}
				    }
				  );
		}
	, MakeDropZoneOf		: function(htmlElement, nurseId) {
		 var self = this;
		 htmlElement.addEventListener( 'dragover'
									 , function(evt) {
										 if(self.draggingPatientCard) {evt.preventDefault();}
										}
									 , false );
		 htmlElement.addEventListener( 'dragenter'
									 , function(evt) {htmlElement.classList.add('currentDropZone');}
									 , false );
		 htmlElement.addEventListener( 'dragleave'
									 , function(evt) {htmlElement.classList.remove('currentDropZone');}
									 , false );
		 htmlElement.addEventListener( 'drop'
									 , function(evt) {
										 if(self.draggingPatientCard) {
											 self.draggingPatientCard.htmlElement.parentNode.removeChild( self.draggingPatientCard.htmlElement );
											 htmlElement.appendChild( self.draggingPatientCard.htmlElement );
											 // Is there some patientCard in the remaining zone ?
											 var L = self.div_remainingPatient.querySelectorAll('.patientCard');
											 if ( (L.length  >  0 && self.div_remainingPatient.classList.contains('display') === false)
												||(L.length === 0 && self.div_remainingPatient.classList.contains('display') === true) ) {
												 self.div_remainingPatient.classList.toggle('display');
												}
											// Affectation of the patient to a new nurse
											 var patient		= self.draggingPatientCard.patient
											   , patientNumber	= patient.querySelector('numéro').textContent;
											 utils.XHR( 'POST', '/affectation'
													  , { onload		: function() {console.log("/affectation =>", this);}
													    , variables	: {infirmier:nurseId, patient: patientNumber}
													    }
													  );
											}
										}
									 , false );
		}
	, editPatient			: function(patient) {
		 var div_addPatient = document.getElementById('addPatient');
		 
		 div_addPatient.classList.add('display');
		 document.getElementById('patientName').value		= patient.querySelector('nom').textContent;
		 document.getElementById('patientForname').value	= patient.querySelector('prénom').textContent;
		 document.getElementById('patientSex').value		= patient.querySelector('sexe').textContent;
		 document.getElementById('patientNumber').value		= patient.querySelector('numéro').textContent;
		 document.getElementById('patientBirthday').value	= patient.querySelector('naissance').textContent;
		 document.getElementById('patientFloor').value		= patient.querySelector('étage').textContent;
		 document.getElementById('patientStreet').value		= patient.querySelector('rue').textContent;
		 document.getElementById('patientPostalCode').value	= patient.querySelector('codePostal').textContent;
		 document.getElementById('patientCity').value		= patient.querySelector('ville').textContent;
		}
	, addPatientUI			: function( patient) {
		 var self = this;
		// Create a root div
		 var div = document.createElement('div');
			div.classList.add('patientCard');
			div.setAttribute('draggable', 'true');
			div.setAttribute('id', patient.querySelector('numéro').textContent);
			div.addEventListener	( 'dragstart'
									, function() {self.draggingPatientCard = {htmlElement: div, patient: patient};}
									, false );
			div.addEventListener	( 'dragend'
									, function() {self.draggingPatientCard = null;}
									, false );
			
		// Name
		 var divName = document.createElement('div');
			divName.classList.add('name');
			divName.appendChild( document.createTextNode(patient.querySelector('nom').textContent) );
			div.appendChild( divName );
		// Forname
		 var divForname = document.createElement('div');
			divForname.classList.add('forname');
			divForname.appendChild( document.createTextNode(patient.querySelector('prénom').textContent) );
			div.appendChild( divForname );
		// return the root div
		 return div;
		}
	, display_Visites		: function( LV, index ) {
		 var self = this;
		 index = index || 0;
		 if(index >= LV.length) {
			 // Display path
			 var request =	{ origin		: this.L_markers[0].position
							, destination	: this.L_markers[0].position
							, travelMode	: google.maps.TravelMode.DRIVING
							, waypoints		: []
							, optimizeWaypoints: false
							}
			 for(var i=1; i<LV.length; i++) {
				 request.waypoints.push( {location : this.L_markers[i].position} );
				}
			 this.serviceDirection.route( request
										, function(response, status) {
											 console.log("Direction:", response, status);
											 self.directionsDisplay.setDirections(response);
											}
										);
			 return;
			}
		 var self	= this;
		 var V      = LV.item(index);
		 console.log(V);
		 var address =        V.querySelector('patient > adresse > adresse > rue').innerHTML
					 + ', ' + V.querySelector('patient > adresse > adresse > codePostal').innerHTML
					 + ', ' + V.querySelector('patient > adresse > adresse > ville').innerHTML
					 + ', France';
		 this.geocoder.geocode(
			  {address: address}
			, function(results, status) {
				  if (status == google.maps.GeocoderStatus.OK) {
					var marker = new MarkerWithLabel({
						  map: self.map
						, position: results[0].geometry.location
						, labelContent: V.querySelector('patient > nom').innerHTML
						, labelAnchor: new google.maps.Point(22, 0)
						, labelClass: "MapLabels"
						});
					 self.L_markers.push( marker );
					} else {console.log("error on " + address, status);}
				 // Continue if this was not the last one
				 self.display_Visites(LV, index+1);
				}
			);
		}
	, displayTournee		: function( infirmier ) {
		 // Get geolocation then create a mark
		 console.log( infirmier );
		 var LV = infirmier.querySelectorAll('visites > visite');
		 for(var i in this.L_markers) {
			 this.L_markers[i].setMap(null);
			 delete this.L_markers[i];
			}
		 this.L_markers = [];
		 this.display_Visites( LV );
		}
};

return ControlPanelInteraction;
});