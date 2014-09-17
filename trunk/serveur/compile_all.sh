#!/bin/sh

echo "--------- Compilation proxy -----------"
cd XML_process ; make
cd .. ; make proxy-L3 
echo
echo "------- Compilation simulateur de serveur HTTP ---------"
gcc -Wall -o simu_serveur_http simu_serveur_http.c
echo
echo "-------- Compilation simulateur de client infirmiere --------"
gcc -Wall -o simu_client_req6 simu_client_req6.c
echo "FINI"

