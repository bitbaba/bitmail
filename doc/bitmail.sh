#!/bin/bash
cat <<_heredoc >/tmp/bitmail.ext
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints=critical,CA:true
keyUsage=critical,digitalSignature,keyCertSign,keyEncipherment
extendedKeyUsage=emailProtection
nsCertType=emailCA,email
_heredoc
echo subjectAltName=email:$1 >> /tmp/bitmail.ext
echo issuerAltName=email:$1 >> /tmp/bitmail.ext


mkdir -p ./craft && \
openssl genrsa -out ./craft/bitmailkey.pem -aes256 2048 && \
openssl req -new -key ./craft/bitmailkey.pem  -subj "/emailAddress=$1" -out ./craft/bitmailcsr.pem && \
openssl x509 -req -in ./craft/bitmailcsr.pem -extfile /tmp/bitmail.ext -signkey ./craft/bitmailkey.pem -days 3650 -CAcreateserial -out ./craft/bitmailcert.crt && \
openssl pkcs12 -export -aes256 -in ./craft/bitmailcert.crt -inkey ./craft/bitmailkey.pem -out ./craft/bitmail.p12
