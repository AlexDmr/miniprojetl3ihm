
Projet à tester sous Linux
    
    A) Installer  les librairies nécessaires
- installation de curl (pour traduire les requêtes GoogleMap)
    > 
- installation de libXML++ 2.6 (pour la partie Langages pour le Web)
    > sudo apt-get install libxml++-dev
        
- installation des librairies NodeJS
    > aller sur http://nodejs.org/
    > installer les librairies javascript nécessaires

            
    B) Récupération du code de correction sur SVN         
- Récupération du code:
    Créer un répertoire ProjetApplicatifIntegrateur (par exemple) et un sous-répertoire Correction
        > svn checkout https://github.com/AlexDmr/miniprojetl3ihm

- Le code a l'arborescence suivante:
                
                +- .svn    // Répertoire qui stocke les info du système de gestion de version (ne pas toucher)
                |
                +- nodeJS  // Répertoire dans lequel se trouve le serveur NodeJS et les infos XML (base de donnée patients, infirmières, etc.)
                |
                +- serveur // Répertoire dans lequel se trouve le serveur Proxy (à faire par les étudiants), 
                     // la partie RO et une partie DOM pour lire/écrire les données XML utilise pour le proxy, les requêtes 
                
    L'arborescence détaillée est donnée plus bas
    
    
    C) Compiler / Déployer avant de lancer les serveur (à faire une seule fois normalement, ou à chaque changement de code...) 
1- Compiler la partie serveur
    a- Aller dans le répertoire serveur 
        > cd serveur
    b- Exécuter la ligne suivante:
        > ./compile_all.sh

2- Développer le serveur nodeJS 
    a- Aller dans le répertoire nodeJS
        > cd ../nodeJS 
            (si vous étiez dans le répertoire serveur)
    b- Déployer le serveur nodeJS
        > npm install        

    D) Tester le projet lui-même (enfin !!)
0- Technique pratique: ouvrir 2 terminaux. Dans le premier aller dans le répertoire serveur, et dans le deuxième dans le répertoire nodeJS.
1- Lancer le proxy
    Dans le répertoire serveur, lancer la commande
    > ./proxy-L3 -p 8000
2- Lancer le serveur NodeJS
    Dans le répertoire nodeJS, lancer le serveur nodeJS avec la commande
    > node ./staticServeur.js port:8080
3- Lancer Firefox et 
4- configurer le proxy
    -> aller dans Tools->Options (ou Edit->Preferences selon les versions) puis Advanced->Network->Settings
    -> Choisir "Manual proxy configuration"
    -> dans HTTP Porxy: taper localhost 
    -> dans Port: taper 8000
5- interdire la persistance des connections
    -> taper about:config dans la barre d'url
    -> dans network.http.proxy.version mettre 1.0 
    -> dans network.http.keep-alive.timeout mettre 0 


4- Dans NetBeans, lancer le client localhost
   -> dans l'URL, taper "http://localhost:8080"

Vous devriez obtenir la page d'accueil.
En cliquant sur "se connecter en tant que secrétaire médicale", on devrait obtenir un message GET dans le terminal où le proxy est lancé.
                 
                
                
                
                
                
                
- Arborescence générale du projet                
                +- .svn // répertoire qui stocke les info du système de gestion de version (ne pas toucher)
                |
                +- nodeJS
                    |
                    +- package.json   // fonctions utiles javascript
                    |
                    +- start.html     // Page d'accueil du site web client'
                    |
                    +- secretary.html // Interface client de la secrétaire médicale  
                    |
                    +- staticSerer.js // gros fichier contenant les instructions du serveur nodeJS
                    |
                    +- css            // Répertoire de fichiers CSS
                    |   |
                    |   +-
                    |
                    +- data
                    |   |
                    |   +- actes.xml   // Fichier d'aide pour le LW'
                    |   |
                    |   +- cabinet.xsd // Schéma de validation du format XML utilisé
                    |   |
                    |   +- cabinetInfirmier.xml // La grosse base de donnée (le principal travail de Langages pour le Web)
                    |
                    +- js 
                        |
                        +- ControlPanelInteraction.js // Fonctions javascript qui....
                        |
                        +- ...
    ... A finir, dès que j'aurai 5min