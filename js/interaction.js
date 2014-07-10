define	( [ 'L3_List'
		  , 'MarkerWithLabel'
		  ]
		, function(L3_List, MarkerWithLabel) {
var Tournee = {
	  map		: null
	, L_propositions		: []
	, L_markers				: []
	, distMatrix			: null
	, serviceMatrix			: null
	, serviceDirection		: null
	, init					: function(mapId, pathId) {
		 var self = this;
		 this.map = new google.maps.Map	( document.getElementById(mapId)
										, { center: new google.maps.LatLng(45.193861, 5.768843)
										  , zoom: 12
										  }
										);
		 google.maps.event.addListener(this.map, 'click', function(e) {self.addMark(e);});
		 this.serviceMatrix	= new google.maps.DistanceMatrixService();
		 this.serviceDirection	= new google.maps.DirectionsService();
		 this.directionsDisplay = new google.maps.DirectionsRenderer({ draggable		: false
																	 , suppressMarkers	: true
																	 , preserveViewport	: true 
																	 });
			this.directionsDisplay.setMap( this.map );
		 
		 // Geocoder
		 this.geocoder = new google.maps.Geocoder();
		 
		 // Create the list of paths
		 this.L_path = (new L3_List()).setTitle( "Les étapes dans l'ordre de passage" );
		 this.L_path.plugUnder( document.getElementById(pathId) );
		 this.L_path.subscribeToReorder( function(li) {
			 console.log(li, "has been reordered");
			 var LIs = self.L_path.getItems(), item;
			 for(var i=0; i<LIs.length; i++) {
				 item = LIs.item(i);
				 item.googleMarker.set('labelContent', i+1);
				}
			});
		 // Init distance matrix
		 this.btDistanceMatrix = document.createElement('button');
			document.getElementById(pathId).appendChild( this.btDistanceMatrix );
			this.btDistanceMatrix.innerText = 'Recalculer les distances';
			this.btDistanceMatrix.addEventListener( 'click'
												  , function() {self.getDistanceMatrix();}
												  , false );
		 // Plug some interaction to update path rendering on the map
		 this.btRenderPath = document.createElement('button');
			document.getElementById(pathId).appendChild( this.btRenderPath );
			this.btRenderPath.innerText = 'Afficher chemin';
			this.btRenderPath.addEventListener(
				  'click'
				, function() {
					 // Compute indexes
					 var L_indexes	= []
					   , LIs 		= self.L_path.getItems();
					 for(var i=0; i<self.L_markers.length; i++) {
						 // Where is positioned this marker in the current list ?
						 var pos = 0;
						 while( pos < LIs.length
							  &&LIs.item(pos).googleMarker !== self.L_markers[i] ) pos++;
						 if(pos >= LIs.length) {throw("Un marker n'est plus référencé dans la liste!!!");}
						 L_indexes.push( pos );
						}
					 // Display 
					 self.displayPath( L_indexes );
					}
				, false );
		}
	, addMark				: function(e) {
		 console.log(e);
		 var self = this
		   , mark = new MarkerWithLabel(
						{ position: e.latLng
						, map: this.map
						, title: 'Mark ' + this.L_markers.length
						, draggable: true
						, raiseOnDrag: true
						, labelContent: this.L_markers.length + 1
						, labelAnchor: new google.maps.Point(22, 0)
						, labelClass: "MapLabels"
						} );
		 this.L_markers.push(mark);
		 // Subscribe to dragging
		 var li = this.L_path.append( mark.position.toString() );
			mark.L3_List_li = li
			li.googleMarker = mark;
			pipo = mark; //XXX
		 this.updateAdress(mark, li);
		 google.maps.event.addListener( mark, 'dragend'
									  , function(e) {
											 console.log("updating", mark, li);
											 self.updateAdress(mark, li);
											}
									  );
		}
	, updateAdress			: function(marker, li) {
		 var self = this;
		 li.innerText = marker.position.toString();
		 this.geocoder.geocode(
			  {'latLng': marker.position}
			, function(results, status) {
				if(status == google.maps.GeocoderStatus.OK) {
					if(results[0]) {
						 li.innerText = results[0].formatted_address;
						} else	{li.innerText = mark.position.toString();
								}
					} else {li.innerText = status;}
			});
		}
	, displayPath			: function(L_indexes) {
		 if(typeof L_indexes === 'undefined') {
			 L_indexes = new Int32Array( this.L_markers.length );
			 for(var i=0; i<this.L_markers.length; i++) {L_indexes[i] = i;}
			}
		 // Build path array
		 var request =	{ origin		: this.L_markers[L_indexes[0]].position
						, destination	: this.L_markers[L_indexes[0]].position
						, travelMode	: google.maps.TravelMode.DRIVING
						, waypoints		: []
						, optimizeWaypoints: false
						}
		   , self = this;
		 for(var i=1; i<L_indexes.length; i++) {
			 request.waypoints.push( {location : this.L_markers[L_indexes[i]].position} );
			}
		 this.serviceDirection.route( request
									, function(response, status) {
										 console.log("Direction:", response, status);
										 self.directionsDisplay.setDirections(response);
										}
									);
		}
	, getDistanceMatrixAtom	: function(L, originPos, destinationPos) {
		 var self		  = this, originPosTmp = originPos, destinationPosTmp = destinationPos;
		 var size		  = Math.min(L.length-destinationPos, 25);
		 var nb			  = size>0?Math.floor(100/size):0
		 if(originPos >= L.length) {
			 originPos			= 0;
			 destinationPos	   += size;
			 originPosTmp		= originPos;
			 destinationPosTmp	= destinationPos;
			 size		  		= Math.min(L.length-destinationPos, 25);
			 nb					= size>0?Math.floor(100/size):0
			}
		 console.log("getDistanceMatrixAtom", L, originPos, destinationPos);
		 if( originPos >= L.length || destinationPos >= L.length) {
			 console.log("End of recursion for retrieving distance matrix", originPos, destinationPos);
			 return;
			}
		 // take care that
		 //		- max origin		: 25
		 //		- max destinations	: 25
		 //		- max origin*dest	: 100
		 //		- delay of request
		 var origins	  = []; for(var i=0; i<nb&&originPos<L.length; i++) {origins.push( L[originPos++] );}
		 var destinations = []; for(var i=0; i<size&&i+destinationPos<L.length; i++) {destinations.push( L[i+destinationPos] );}
		 console.log("Requesting <", originPosTmp, ';', destinationPosTmp, ';', origins.length, ';', destinations.length, '>');
		 this.serviceMatrix.getDistanceMatrix(
			{ origins		: origins
			, destinations	: destinations
			, travelMode	: google.maps.TravelMode.DRIVING
			, unitSystem	: google.maps.UnitSystem.METRIC
			}, function(response, status) {
				 // console.log("getDistanceMatrix:", response, status);
				 if(status === "OK") {
					 var row, dist;
					 for(var i=0; i<response.rows.length; i++) {
						 row = response.rows[i].elements;
						 console.log(row);
						 for(var j=0; j<row.length; j++) {
							 dist = row[j];
							 self.distMatrix[originPosTmp+i][destinationPosTmp+j] = dist.duration.value;
							 console.log(originPosTmp+i, "->", destinationPosTmp+j, ":", self.distMatrix[originPosTmp+i][destinationPosTmp+j]);
							}
						}
					 setTimeout( function() {self.getDistanceMatrixAtom(L, originPos, destinationPos);}
							   , 10000 );
					} else {console.error("Google Map error", status, response);}
				}
			);
		}
	, getDistanceMatrix		: function() {
		 var L = [], self = this;
		 for(var i=0; i<this.L_markers.length; i++) {
			 console.log( this.L_markers[i] );
			 L.push( this.L_markers[i].position );
			}
		 // Initialize the distMatrix array
		 this.distMatrix = new Array( L.length );
		 for(var i=0; i<this.distMatrix.length; i++) {
			 this.distMatrix[i] = new Int32Array( this.distMatrix.length );
			}
		// Request
		 this.getDistanceMatrixAtom(L, 0, 0);
		}
	, displayMarker			: function(L) {
		 if(L.length === 0) {return;}
		 var self = this
		   , ad   = L.splice(0,1);
		 console.log(ad[0]);
		 this.geocoder.geocode(
			  {'address': ad[0] + ", france"}
			, function(results, status) {
				  if (status == google.maps.GeocoderStatus.OK) {
					var marker = new google.maps.Marker({
						 map: self.map,
						 position: results[0].geometry.location
						});
					} else {console.log("error", status);}
				 // console.log("go on with", L);
				 setTimeout(function(){self.displayMarker(L);}, 500);
				}
			);
		}
};

return Tournee;
});