#!/bin/sh

echo "------------- Compilation API XML -------------"
cd XML_process ; make
success=$?
echo
[ $success -eq 0 ] && (echo "------------ Compilation proxy --------------"; cd .. ; make proxy-metier)
success=$?
echo
if [ $success -eq 0 ]
then
echo "COMPILATION FINIE"
else 
echo "ERREUR(S)"
fi


