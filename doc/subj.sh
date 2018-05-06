#!/bin/bash
#ref: https://www.ietf.org/mail-archive/web/pkix/current/msg05382.html
#ref: https://stackoverflow.com/questions/3468127/x509-whats-the-difference-between-digital-signature-and-non-repudiation
#ref: https://security.stackexchange.com/questions/6730/what-is-the-difference-between-authenticity-and-non-repudiation

# The keyEncipherment bit is asserted when the subject public key is used for key transport.  For example, when an RSA key is to be used for key management by encrypting a symmetric content-encryption key, then this bit shall asserted.
# The dataEncipherment bit is asserted when the subject public key is used for directly enciphering raw user data without the use of an intermediate symmetric cipher.

# digitalSignature: when you parter receive a signature informations(e.g. sig/msg/cert), (s)he can be convinced that it from the identity of the cert.
# non-repudiation: when you are shown siginfo(e.g. sig/msg/cert), you can not deny that you signed it. 
# To avoid this:
# [1] signing tool (using a cert flagged with non-repudiation) should prompt you what you are going to sign, EVERY TIME. 
# or
# [2] before you flag your cert (or use a issued cert) with non-repudiation, you convince everyone(incluing yourself) that you absolutly have kept your key safely.

# digitalSignature: yes, sig was signed by the cert(private key included).
# non-repudiation: and the key is only used under the intention of the owner. how to proof? i dont think it's computable.

cat <<_heredoc >/tmp/subj.ext
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid:always,issuer
basicConstraints=critical,CA:false
keyUsage=critical,digitalSignature,keyEncipherment
extendedKeyUsage=emailProtection
nsCertType=email
#subjectAltName=email:example@mail.com
_heredoc
echo subjectAltName=email:$1 >> /tmp/subj.ext

mkdir -p ./craft && \
openssl genrsa -out ./craft/subjkey.pem -aes256 2048 && \
openssl req -new -key ./craft/subjkey.pem -subj "/emailAddress=$1/CN=$2/O=bitbaba.com/OU=bitmail/C=CN/S=SH" -out ./craft/subjcsr.pem && \
openssl x509 -req -in ./craft/subjcsr.pem -extfile /tmp/subj.ext -CA ./craft/cacert.crt -CAkey ./craft/cakey.pem -days 3650 -CAcreateserial -out ./craft/subjcert.crt && \
openssl pkcs12 -export -aes256 -in ./craft/subjcert.crt -inkey ./craft/subjkey.pem -certfile ./craft/cacert.crt -out ./craft/subj.p12 
