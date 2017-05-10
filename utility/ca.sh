#!/bin/bash

nick=$1 #"Harry QMail"
email=$2 #"751302230@qq.com"
org="bitmail.bitbaba.com"
ou="bitmail dept. of bitbaba corp."

openssl genrsa -aes256 -out cakey.pem 2048 

openssl req -new -key cakey.pem -subj "/CN=$nick/emailAddress=$email/O=$org/OU=$ou" -out cacsr.pem

cat <<heredoc >v3.ext
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints=CA:true
keyUsage = digitalSignature, keyEncipherment, dataEncipherment, keyCertSign, cRLSign
nsCertType = emailCA, email, sslCA
heredoc

#echo extendedKeyUsage=serverAuth, clientAuth, emailProtection, codeSigning, timeStamping, OCSPSigning
echo extendedKeyUsage=emailProtection >> v3.ext
echo subjectAltName=email:$email >> v3.ext
echo issuerAltName=email:$email  >> v3.ext

openssl x509 -req -in cacsr.pem -extfile v3.ext -signkey cakey.pem -days 3650 -CAcreateserial -out cacert.pem

openssl pkcs12 -export -aes256 -in cacert.pem -inkey cakey.pem -certfile cacert.pem -out ca.p12

