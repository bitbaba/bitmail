                 ____    _   _     __  __           _   _ 
                | __ )  (_) | |_  |  \/  |   __ _  (_) | |
                |  _ \  | | | __| | |\/| |  / _` | | | | |
                | |_) | | | | |_  | |  | | | (_| | | | | |
                |____/  |_|  \__| |_|  |_|  \__,_| |_| |_|

                
                

[![license](https://img.shields.io/badge/license-BSD-green.svg?style=flat)](https://github.com/imharrywu/bitmail/edit/master/LICENSE)

# Download pre-built installer for windows

http://imharrywu.github.io/bitmail-qt/bitmail-qt-win32-installer.exe

# Introduction

- Bitmail(比特信) is a purly Peer-to-Peer instant messenger client, to help people to communicate with each other securely. 

- Authentication and security are based on PKI(Public Key Infrastructure), and are powered by [OpenSSL](https://github.com/openssl/openssl). 

- A Message singed with your private key is definitely sent from yourself, not from a middle-man.

- A Message encrypted with your friend's certificate, is not viewed by anyone while transferring.

- The rx/tx network is now based on MX(eMail eXchange) protocols, e.g. imap(s) & smtp(s), and is powered by [CURL](https://github.com/bagder/curl). There is more delay than real IM or IRC network. But the MX network can be replaced by (or upgraded to) other transfer networks. 

- Free-speaking is your right. Get the privacy-protection from operators back to yourself.

- White paper: [bitmail.pdf.md](./doc/bitmail.pdf.md)
    
# Features
- secure talking with trusted buddies.
- PKI utilities used to sign images, documents, and vote.
- reject any spam message away.
- invite friend(s) or accept friend(s) by exchange CertID.

# Build

## clone

```
$git clone https://github.com/imharrywu/bitmail
```

## build depends

```
$cd depends
$sh openssl.sh && sh curl.sh && sh libmicrohttpd.sh
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
                           
