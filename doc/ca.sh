#!/bin/bash
cat <<_heredoc >/tmp/caroot.ext
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints=critical,CA:true
keyUsage=critical,digitalSignature,keyCertSign
#extendedKeyUsage=serverAuth,clientAuth,codeSigning,emailProtection,timeStamping,OCSPSigning
nsCertType=emailCA
subjectAltName=email:rootca@bitmail.bitbaba.com,DNS:bitmail.bitbaba.com,URI:https://bitmail.bitbaba.com/
issuerAltName=email:rootca@bitmail.bitbaba.com,DNS:bitmail.bitbaba.com,URI:https://bitmail.bitbaba.com/
_heredoc

mkdir -p ./craft && \
openssl genrsa -out ./craft/cakey.pem -aes256 2048 && \
openssl req -new -key ./craft/cakey.pem  -subj "/CN=bitmail.bitbaba.com" -out ./craft/cacsr.pem && \
openssl x509 -req -in ./craft/cacsr.pem -extfile /tmp/caroot.ext -signkey ./craft/cakey.pem -days 3650 -CAcreateserial -out ./craft/cacert.crt && \
openssl pkcs12 -export -aes256 -in ./craft/cacert.crt -inkey ./craft/cakey.pem -out ./craft/ca.p12
