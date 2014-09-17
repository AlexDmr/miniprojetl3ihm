define( function() {
var L3_List = function( L ) {
	 this.init();
	 L = L || [];
	 for(var i=0; i<L.length; i++) {this.append( L[i] );}
	 return this;
	}
L3_List.prototype.constructor = L3_List;
L3_List.prototype.init = function() {
	 this.L_CB_Reorder = [];
	 this.root	= document.createElement('div');
		this.root.classList.add( 'L3_List' );
	 this.ul    = document.createElement('ul');
	 this.title = document.createElement('div');
	 this.root.appendChild	( this.title );
	 this.root.appendChild	( this.ul    );
	}
L3_List.prototype.getItems = function() {
	return this.ul.children;
}
L3_List.prototype.setTitle = function(element) {
	 this.title.innerText = '';
	 if( element.constructor === String
	   ||element.constructor === Number) {
		 this.title.innerText = element;
		} else {this.title.appendChild(element);}
	 return this;
	}
L3_List.prototype.plugUnder = function(node) {
	 node.appendChild( this.root );
	 return this;
	}
L3_List.prototype.append = function(element) {
	 var li   = document.createElement('li')
	   , self = this;
		li.setAttribute('draggable', 'true');
	 if( element.constructor === String
	   ||element.constructor === Number) {
		 li.innerText = element;
		} else {li.appendChild(element);}
	 this.ul.appendChild(li);
	 li.addEventListener( 'dragstart'
						, function(e) {return self.dragStart(e);}
						, false);
	 li.addEventListener( 'dragover'
						, function(e) {return self.dragover(e);}
						, false);
	 return li;
	}
// Drag and drop
L3_List.prototype.subscribeToReorder 		= function ( CB ) {
	if(this.L_CB_Reorder.indexOf(CB) === -1) {this.L_CB_Reorder.push(CB);}
}
L3_List.prototype.unSubscribeFromReorder	= function ( CB ) {
	var pos = this.L_CB_Reorder.indexOf(CB);
	if(pos >= 0) {this.L_CB_Reorder.splice(pos,1);}
}
L3_List.prototype.CallBackReorder			= function (li)	  {
	for(var i=0; i<this.L_CB_Reorder.length; i++) {
		 this.L_CB_Reorder[i].apply(this, [li]);
		}
}

L3_List.prototype.dragStart = function(e) {
	 this.dragged = e.currentTarget;
	 e.stopPropagation();
	}
L3_List.prototype.dragover = function(e) {
	 if(this.dragged && this.dragged !== e.currentTarget) {
		 var bbox			  = e.currentTarget.getBoundingClientRect()
		   , currentTargetPos, draggedPos;
		 for(var i=0; i<this.ul.children.length; i++) {
			 if( this.ul.children.item(i) === e.currentTarget ) {currentTargetPos = i;}
			 if( this.ul.children.item(i) === this.dragged    ) {draggedPos       = i;}
			}
		 if(  e.clientY < bbox.top + bbox.height/2 ) {
			 if( draggedPos !== currentTargetPos - 1 ) {
				 // console.log('Cas 1:');
				 this.ul.removeChild( this.dragged );
				 this.ul.insertBefore(this.dragged, e.currentTarget);
				 this.CallBackReorder( this.dragged );
				}
			} else	{var child = this.ul.children.item( currentTargetPos + 1 );
					 if(child) {
						 if( draggedPos !== currentTargetPos + 1) {
							 // console.log('Cas 2:', draggedPos, currentTargetPos);
							 this.ul.removeChild( this.dragged );
							 this.ul.insertBefore(this.dragged, child);
							 this.CallBackReorder( this.dragged );
							}
						} else {if(draggedPos !== this.ul.children.length - 1) {
									 // console.log('Cas 3:', draggedPos, currentTargetPos);
									 this.ul.removeChild( this.dragged );
									 this.ul.appendChild( this.dragged );
									 this.CallBackReorder( this.dragged );
									}
							   }
					}
		}
	 e.preventDefault();
	}
L3_List.prototype.dragEnd  = function(e) {
	 this.dragged = null;
	}

return L3_List;
});