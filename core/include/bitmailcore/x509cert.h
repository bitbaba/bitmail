#ifndef X509CERT_H
#define X509CERT_H

#include <string>
#include <vector>
#include <openssl/objects.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs7.h>
#include <openssl/cms.h>
#include <openssl/stack.h>
#ifndef OPENSSL_NO_ENGINE
# include <openssl/engine.h>
#endif

class CX509Cert
{
public:
    CX509Cert();
    CX509Cert(const std::string & certpem);
    CX509Cert(const std::string & certpem, const std::string & privkey, const std::string & passphrase);

public:
    bool Create(unsigned int bits, const std::string & commonName, const std::string & email, const std::string & passphrase);
    bool ImportCert(const std::string & cert);
    bool ImportPrivKey(const std::string & key, const std::string & passphrase);
    bool ImportCertFromSig(const std::string & sig);
    std::string ExportCert() const;
    std::string ExportPrivKey() const;
    std::string ExportPKCS12() const;
    bool IsValid() const;
    std::string GetPassphrase() const;
    bool SetPassphrase(const std::string & passphrase);
    std::string GetEmail() const ;
    std::string GetCommonName() const;
    std::string GetID() const;
    unsigned int GetBits() const;

protected:
    int MakeCert(const std::string & commonName, const std::string & email, const std::string & passphrase, unsigned int bits);
    int AddExt(X509 *cert, int nid, char *value);

protected:
    static std::string PrivKey2PEM(const EVP_PKEY * pkey, const std::string & passphrase);
    static std::string Cert2PEM(const X509 * cert);
    static X509 * PEM2Cert(const std::string sCert);
    static EVP_PKEY * PEM2PrivKey(const std::string & sRsa, const std::string & passphrase);

public:
    static std::string GetSigningTime(const std::string & sig);
    static int CheckMsgType(const std::string & msg);
    static std::string MEncrypt(const std::string & msg, const std::set<std::string> & certs);
    static std::string MSign(const std::string & msg, const std::vector<CX509Cert> & signers);

public:
    /*Certificate*/
    std::string Sign(const std::string & msg);
    std::string Verify(const std::string & msg);
    std::string Encrypt(const std::string & msg);
    std::string Decrypt(const std::string & msg);

    /*Public Key*/
    std::string PKSign(const std::string & msg, const std::string & dgst);
    std::string PKVerify(const std::string & msg, const std::string & dgst, const std::string & sig);
    std::string PKEncrypt(const std::string & msg);
    std::string PKDecrypt(const std::string & code);

    /*Symmetric Key*/
    static std::string SKEncrypt(const std::string & msg, const std::string & algo, const std::string & secret);
    static std::string SKDecrypt(const std::string & code, const std::string & algo, const std::string & secret);

public:
    static std::string hash(const std::string & str, const std::string & algo);
    static std::string b64enc(const std::string & str, bool crlf);
    static std::string b64dec(const std::string & b64, bool crlf);
private:
    // Ref: [https://www.openssl.org/docs/crypto/d2i_X509.html]
    // PEM encoded string for Certificate
    std::string m_cert;
    // Ref: [https://www.openssl.org/docs/crypto/i2d_RSA_PUBKEY.html]
    // PEM encoded string for Private Key
    std::string m_key;
    // passphrase for private key
    std::string m_passphrase;
};

#endif // X509CERT_H
