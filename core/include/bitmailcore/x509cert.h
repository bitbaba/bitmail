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
public:
    int Create(const std::string & commonName, const std::string & email, const std::string & passphrase, unsigned int bits);
    int LoadCertFromPem(const std::string & cert);
    int LoadKeyFromEncryptedPem(const std::string & key, const std::string & passphrase);
    int LoadCertFromSig(const std::string & sig);

protected:
    int MakeCert(const std::string & commonName, const std::string & email, const std::string & passphrase, unsigned int bits);
    int AddExt(X509 *cert, int nid, char *value);

protected:
    static std::string PKeyToPem(const EVP_PKEY * pkey, const std::string & passphrase);
    static std::string CertToPem(const X509 * cert);
    static EVP_PKEY * PemToPKey(const std::string & sRsa, const std::string & passphrase);
    static X509 * PemToCert(const std::string sCert);

public:
    static int CheckMsgType(const std::string & msg);
    static std::string MEncrypt(const std::string & msg, const std::vector<CX509Cert> & certs);
    static std::string MSign(const std::string & msg, const std::vector<CX509Cert> & signers);

public:
    bool IsValid() const;
    X509 * GetCert() const;
    std::string GetCertByPem() const;
    std::string GetEmail() const ;
    std::string GetCommonName() const;
    std::string GetID() const;
    int GetBits() const;
    std::string GetPassphrase() const;
    int SetPassphrase(const std::string & passphrase);

    EVP_PKEY * GetPrivateKey();
    std::string GetPrivateKeyAsEncryptedPem();

    std::string Sign(const std::string & msg);
    std::string Verify(const std::string & msg);
    std::string Encrypt(const std::string & msg);
    std::string Decrypt(const std::string & msg);

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
