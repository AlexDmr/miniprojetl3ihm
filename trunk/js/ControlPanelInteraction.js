define	( [ 'L3_List'
		  , 'MarkerWithLabel'
		  , 'utils'
		  ]
		, function(L3_List, MarkerWithLabel, utils) {
var ControlPanelInteraction = {
	  map					: null
	, L_markers				: []
	, init					: function(mapId, tableInfirmiereId) {
		 var self = this;
		 this.map = new google.maps.Map	( document.getElementById(mapId)
										, { center: new google.maps.LatLng(45.193861, 5.768843)
										  , zoom: 12
										  }
										);
		 // google.maps.event.addListener(this.map, 'click', function(e) {self.addMark(e);});
		 this.serviceDirection	= new google.maps.DirectionsService();
		 this.directionsDisplay = new google.maps.DirectionsRenderer({ draggable		: false
																	 , suppressMarkers	: true
																	 , preserveViewport	: true 
																	 });
		 this.directionsDisplay.setMap( this.map );
		 
		 // Geocoder
		 this.geocoder = new google.maps.Geocoder();

		 // Access to the server data
		 this.tableInfirmiere     = document.getElementById( tableInfirmiereId );
		 this.bodytableInfirmiere = this.tableInfirmiere.querySelector('tbody');
		 utils.XHR( 'GET', '/data/cabinet_output.xml'
				  , {onload : function() {
								 var xml = this.responseXML;
								 self.dataXML = xml;
								 // console.log( xml );
								 var L = xml.querySelectorAll('infirmiers > infirmier');
								 // console.log( L );
								 for(var i=0; i<L.length; i++) {
									 var tr = document.createElement('tr');
									 var str = "<td>" 
											 + L.item(i).querySelector('prénom').innerHTML
											 + " "
											 + L.item(i).querySelector('nom').innerHTML
											 + "</td>"
									 str += "<td>";
										// List all visite
										var LP = L.item(i).querySelectorAll('visite');
										// console.log(LP);
										for(var p=0; p<LP.length; p++) {
											 str += '<span class="visite">';
											 str += LP.item(p).querySelector('prénom').innerHTML
												  + ' '
												  + LP.item(p).querySelector('nom').innerHTML;
											 str += "</span> -> ";
											}
									 str += "</td>";
									 tr.innerHTML = str;
									 self.bodytableInfirmiere.appendChild( tr );
									 
									 // Subscribe to mousedown
									 tr.addEventListener( 'mousedown'
														, function(i) {
															return function(e) {
																	 self.displayTournee( L.item(i) );
																	}
															}(i)
														, false);
									}
								}
				    }
				  );
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