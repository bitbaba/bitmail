
>Abstract: 
>a purly peer-to-peer secure messenger client would allow online communications with privacy-protection. 
> Traditional email exchagne system with PGP security support provide communications for privacy protection. 
> Bitmail is a modified version of email-client with PKI security support.


# Introduction
Bitmail can be understood as an invention as bit+mail. In which, bit means encryption, and mail means email. Bitmail core library has two components: the CX509Cert class and the CEmailClient class.

# Certificate
Certificate operations are implemented in CX509Cert class, including:
- Gerneration of certificate
- Encryption and Decryption
- Signing and Verifying
- Protect confidential settings
- User info storage

# Network
Network operations are implemented in CEMailClient class, including:
- IMAP receving
- SMTP sending
- BRAd and NAT-PMP

### details
- message sending and receving
- BRAd protocols

# Message
- PeerMessage

```
a message sent from a peer to anther peer
```

- GroupMessage
```
a message sent from a peer to a group a peer, with groupid, groupname etc.
```

- SubscribeMessage
```
a message sent from a peer to all subscribers
```

# Payment
- integration with bitcoin
