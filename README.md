                 ____    _   _     __  __           _   _ 
                | __ )  (_) | |_  |  \/  |   __ _  (_) | |
                |  _ \  | | | __| | |\/| |  / _` | | | | |
                | |_) | | | | |_  | |  | | | (_| | | | | |
                |____/  |_|  \__| |_|  |_|  \__,_| |_| |_|

                
                

[![license](https://img.shields.io/badge/license-BSD-green.svg?style=flat)](https://github.com/imharrywu/bitmail/edit/master/LICENSE)

# Download pre-built installer for windows

[win32-release](https://github.com/bitbaba/bitmail/releases/download/win32/bitmail-qt-win32-installer.exe)

# Introduction

- Bitmail(比特信) is a purly Peer-to-Peer instant messenger client, to help people to communicate with each other securely. 

- Authentication and security are based on PKI(Public Key Infrastructure), and are powered by [OpenSSL](https://github.com/openssl/openssl). 

- A Message singed by a signer's private key identify the sender (see Middle-Man Attack).

- A Message encrypted by public key, can not be viewed by anyone without private key while transferring.

- The rx/tx network is now based on MX(eMail eXchange) protocols, e.g. imap(s) & smtp(s), and is powered by [CURL](https://github.com/bagder/curl). There is more delay than real IM or IRC network. But the MX network can be replaced by (or upgraded to) other transfer networks. 

- Free-speaking is your right. Get the privacy-protection from operators back to yourself.

- White paper: [bitmail.pdf.md](./doc/bitmail.pdf.md)
    
# Features
- secure talking with trusted friends.
- PKI utilities used to sign images, documents, and voting.
- reject any spam message away.
- invite friend(s) or accept friend(s) by exchange CertID ( the fingerprint of certificate ).

# Build

## clone

```
$git clone https://github.com/imharrywu/bitmail
```

## build depends

```
$cd depends
$sh openssl.sh && sh curl.sh && sh libmicrohttpd.sh && sh miniupnpc.sh && qrencode.sh
```

## build core

```
$sh autogen.sh
```

## build Qt

open \`qt/bitmail.pro' to build


## TODO
- Mobile;
- Daemon & UPnP;
- Bitcoin wallet integration;
- Qt-GUI: translations, rich-text support, recall of history message-queue;


## Welcome to contribute
- Anyone with Qt, web, mobile, or secure network programming knowledge is welcome.
                           
## TODO: ecdsa certificates 
- openssl req -new -x509 -nodes -newkey ec:<(openssl ecparam -name secp384r1) -keyout cert.key -out cert.crt -days 3650
- ref: https://www.guyrutenberg.com/2013/12/28/creating-self-signed-ecdsa-ssl-certificate-using-openssl/
