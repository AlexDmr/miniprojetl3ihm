/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
function afficherFacture(prenom, nom, actes)
{
    var text = "";
    
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
    text +="</table>";

    
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
        var intitule = acte.childNodes[0].nodeValue;
        
        str += "<td>" + typeId + "</td>";
        str += "<td>" + cle + "</td>";
        str += "<td>" + intitule + "</td>";
        str += "<td>" + coef + "</td>";
        str += "<td> 0.0 </td>";
    }
  
  
    return str;
}
