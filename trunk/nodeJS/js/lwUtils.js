/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

function loadXMLDoc(docName)
{
    var xmlhttp;
    if (window.XMLHttpRequest)
    {// code for IE7+, Firefox, Chrome, Opera, Safari
        xmlhttp=new XMLHttpRequest();
    }
    else
    {// code for IE6, IE5
        xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
    }

    xmlhttp.open("GET", docName,false);
    xmlhttp.send();
    xmlDoc=xmlhttp.responseXML;

    return xmlDoc;
}

function adresseToRequestString(adresse)
{
    var str = "";
    str += adresse.getElementsByTagName("ville")[0].childNodes[0].nodeValue;
    str += "+";
    str += adresse.getElementsByTagName("codePostal")[0].childNodes[0].nodeValue;

    // Comme le numéro n'est pas obligatoire (à préciser dans le schéma),
    //  ne l'inclure que s'il existe !
    if (adresse.getElementsByTagName("numéro").length > 0) {
        str += "+";
        str += adresse.getElementsByTagName("numéro")[0].childNodes[0].nodeValue;
    }
    str += "+";
    
    // Pour la rue, il faut remplacer chaque espace par un +
    var strTmp = adresse.getElementsByTagName("rue")[0].childNodes[0].nodeValue;    
    strTmp = strTmp.replace(/ /g, "+");
    
    str += strTmp;
    
   return str;
}

function adresseToText(adresse)
{
    var str = "";
    
    if (adresse.getElementsByTagName("étage").length > 0) {
        str += adresse.getElementsByTagName("étage")[0].childNodes[0].nodeValue + " étage <br/>";
    }
   
    if (adresse.getElementsByTagName("numéro").length > 0) {        
        str += adresse.getElementsByTagName("numéro")[0].childNodes[0].nodeValue + " ";
    }
    
    str += adresse.getElementsByTagName("rue")[0].childNodes[0].nodeValue + "<br/>"; 
    str += adresse.getElementsByTagName("codePostal")[0].childNodes[0].nodeValue + " ";
    str += adresse.getElementsByTagName("ville")[0].childNodes[0].nodeValue + "<br/>";
    
    return str;
}
