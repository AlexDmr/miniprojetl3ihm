
const AMIVAL = 3.15;
const AISVAL = 2.65;
const DIVAL = 10.0;

var totalFacture = 0.0;

function afficherFacture(prenom, nom, actes)
{
    totalFacture = 0.0;
    var text = "<html>\n";
    text += 
    "    <head>\n\
            <title>Facture</title>\n\
            <link rel='stylesheet' type='text/css' href='css/mystyle.css'/>\n\
         </head>\n\
         <body>\n";
                 
    text += "<table>";
    
    text += "<tr>";
    text += "<td>" + prenom + " " + nom + "</td>";
    text += "<td> </td>";
    text += "</tr>";
    
 
    // Trouver l'adresse du patient
    var xmlDoc = loadXMLDoc("data/cabinetInfirmier.xml");
    var patients = xmlDoc.getElementsByTagName("patient");
    var i = 0;
    var found = false;
    while ((i < patients.length) && (!found)) {
        var patient = patients[i];
        var localNom    = patient.getElementsByTagName("nom")[0].childNodes[0].nodeValue;
        var localPrenom = patient.getElementsByTagName("prénom")[0].childNodes[0].nodeValue;
        if ((nom == localNom) && (prenom == localPrenom)) {
            found = true;
        }
        else {
            i++;
        }
    }
    if (found) {
        text += "<tr>";
        text += "<td>";
        var adresse = patients[i].getElementsByTagName("adresse")[0];
        text += adresseToText(adresse);
        text += "</td>";    
        var nSS = patients[i].getElementsByTagName("numéro")[0].childNodes[0].nodeValue;
        text += "<td> Numéro de sécurité sociale: " + nSS + "</td>";
        text += "</tr>";
    }        
    
    text += "</table>"
    
    // Actes
    text += "<br/>";
    text += "<table border='1'  bgcolor='#CCCCCC'>";
    text += "<tr>";
    text += "<td> Type </td> <td> Clé </td> <td> Intitulé </td> <td> Coef </td> <td> Tarif </td>";
    text += "</tr>";
    
    var acteIds = actes.split(" ");
    
    for (var j = 0; j < acteIds.length; j++) {
        text += "<tr>"
        var acteId = acteIds[j];
        text += acteTable(acteId);
        text +="</tr>";
    }
    text += "<tr><td colspan='4'>Total</td><td>" + totalFacture + "</td></tr>\n"
    
    text +="</table>";
    text += 
    "    </body>\n\
    </html>\n"
    
    return text;
}


function acteTable(acteId)
{
    var str = "";
    
    var xmlDoc = loadXMLDoc("data/actes.xml");
    var actes = xmlDoc.getElementsByTagName("acte");
    
    // Trouver l'acte qui correspond
    var i = 0;
    var found = false;
    while ((i < actes.length) && (!found)) {
        var acte = actes[i];
        if (acte.getAttribute("id") == acteId) {
            found = true;
        }
        else {
            i++;
        }
    }
    
    if (found) {
        var acte = actes[i];
        var cle  = acte.getAttribute("clé");
        var coef = acte.getAttribute("coef");
        var typeId = acte.getAttribute("type");
        var type = "";
        // Récupérer la chaîne de caractère du type
        var types = xmlDoc.getElementsByTagName("type");
        var t = 0;
        while ((t < types.length) && (types[t].getAttribute("id") != typeId)) {
            t++;
        }
        if (t < types.length) {
          type = types[t].childNodes[0].nodeValue;
        }
        
        var intitule = acte.childNodes[0].nodeValue;
        // Tarif = (lettre-clé)xcoefficient        
        var tarif = parseFloat(coef);
        
        if (cle == "AMI") {
            tarif *= AMIVAL;
        }
        else if (cle == "AIS") {
            tarif *= AISVAL;
        }
        else if (cle == "DI") {
            tarif *= DIVAL;
        }
          
        str += "<td>" + type + "</td>";
        str += "<td>" + cle + "</td>";
        str += "<td>" + intitule + "</td>";
        str += "<td>" + coef + "</td>";
        str += "<td>" + tarif + "</td>";
        totalFacture += tarif;
    }
  
  
    return str;
}

