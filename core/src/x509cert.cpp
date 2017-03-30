# include <bitmailcore/bitmail_internal.h>
#include <bitmailcore/x509cert.h>
/*
 * Certificate creation. Demonstrates some certificate related operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#if defined(X509_NAME) && (defined(_MSC_VER) || defined(MINGW_DDK_H))
#undef X509_NAME
#endif

CX509Cert::CX509Cert()
    : m_cert("")
    , m_key("")
{

}

int CX509Cert::Create(const std::string & commonName
        , const std::string & email
        , const std::string & passphrase
        , unsigned int bits)
{
    if (MakeCert(commonName, email, passphrase, bits) == 0){
        m_passphrase = passphrase;
        return 0;
    }
    return 1;
}

int CX509Cert::LoadCertFromPem(const std::string & sPemCert)
{
    X509 * x = PemToCert(sPemCert);
    if (x != NULL){
        m_cert = sPemCert;
        return 0;
    }
    return -1;
}

int CX509Cert::LoadKeyFromEncryptedPem(const std::string &key, const std::string &passphrase)
{
    EVP_PKEY * privKey = PemToPKey(key, passphrase);
    if (privKey != NULL){
        m_key = key; m_passphrase = passphrase;
        return 0;
    }
    return -1;
}

/**
* http://linux.die.net/man/3/cms_verify
* https://www.openssl.org/docs/manmaster/crypto/CMS_verify.html
* CMS_get0_signers() retrieves the signing certificate(s) from cms, it must be called after a successful CMS_verify() operation.
* So, there are differences:
* http://linux.die.net/man/3/pkcs7_verify
* PKCS7_get0_signers() retrieves the signer's certificates from p7, it does not check their validity or whether any signatures are valid. The certs and flags parameters have the same meanings as in PKCS7_verify().
*/
int CX509Cert::LoadCertFromSig(const std::string & msg)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) {
        fprintf(stderr, "failed to create memory buffer\n");
        return -1;
    }
    BIO *cont = NULL;
    PKCS7 * cms = NULL;
    STACK_OF(X509) * sk_signers = NULL;

    int ret = 1;
    int flags = 0;

    /* read signature */
    cms = SMIME_read_PKCS7(in, &cont);

    if (!cms){
        fprintf(stderr, "Failed to read cms data\n");
        goto err;
    }

    sk_signers = PKCS7_get0_signers(cms, NULL, 0);

    if (!sk_signers){
        printf("Failed to get singers\n");
    }

    for (int i = 0; i < sk_X509_num(sk_signers); i++){
        X509 * x = sk_X509_value(sk_signers, i);
        m_cert = CertToPem(x);
        break;
    }

    ret = 0;

 err:
    if (ret) {
        fprintf(stderr, "Error load certificate(s)\n");
        ERR_print_errors_fp(stderr);
    }
    PKCS7_free(cms);
    BIO_free(in);
    return 0;
}

int CX509Cert::CheckMsgType(const std::string & msg)
{
    int nid = 0;
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) return -1;

    BIO *cont = NULL;
    CMS_ContentInfo * cms = NULL;


    /* read signature */
    cms = SMIME_read_CMS(in, &cont);

    if (!cms)
        goto err;

    nid = OBJ_obj2nid(CMS_get0_type(cms));

    switch( nid ) {
        case NID_pkcs7_data:
            //printf("p7 type: data\n");
            break;

        case NID_pkcs7_enveloped:
            //printf("p7 type: envloped data\n");
            break;
        case NID_pkcs7_encrypted:
            //printf("p7 type: encryptd data\n");
            break;

        case NID_pkcs7_digest:
            //printf("p7 type: digest\n");
            break;

        case NID_pkcs7_signed:
            //printf("p7 type: singed\n");
            break;

        default:
            //printf("p7 type: unknown \n");
            break;
    }

 err:
    CMS_ContentInfo_free(cms);
    BIO_free(in);

    return nid;
}

int CX509Cert::MakeCert(const std::string & commonName
                    , const std::string & email
                    , const std::string & passphrase
                    , unsigned int bits)
{
    int BITS = (bits & 0xffffff00);

    const int SERIAL = 1;
    const int DAYS   = 1000;

    X509 *x = NULL;
    EVP_PKEY *pk = NULL;
    RSA *rsa = NULL;

    X509_NAME *name = NULL;

    if ((pk = EVP_PKEY_new()) == NULL) {
        goto err_MakeCert;
    }

    if ((x = X509_new()) == NULL){
        goto err_MakeCert;
    }

    rsa = RSA_generate_key(BITS, RSA_F4, NULL, NULL);
    if (!EVP_PKEY_assign_RSA(pk, rsa)) {
        goto err_MakeCert;
    }
    rsa = NULL;

    /**
     * @brief X509_set_version
     * 0 for v1
     * 1 for v2
     * 2 for v3 *
     */
    X509_set_version(x, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x), SERIAL);
    X509_gmtime_adj(X509_get_notBefore(x), 0);
    X509_gmtime_adj(X509_get_notAfter(x), (long)60 * 60 * 24 * DAYS);
    X509_set_pubkey(x, pk);

    name = X509_get_subject_name(x);

    /*
     * This function creates and adds the entry, working out the correct
     * string type and performing checks on its length. Normally we'd check
     * the return value for errors...
     */

    if (X509_NAME_add_entry_by_txt(name, "emailAddress",
                               MBSTRING_ASC
                               , (const unsigned char *)email.c_str()
                               , -1 // by strlen(email.c_str())
                               , -1 // loc, location
                               , 0))// set

    {
        char szEmail [100] = "";
        int rc = X509_NAME_get_text_by_NID(name, NID_pkcs9_emailAddress, szEmail, sizeof(szEmail));
        (void )rc;
        rc = rc;
        char c = *szEmail;
        c = c;
    }

    if (X509_NAME_add_entry_by_txt(name, "commonName",
                               MBSTRING_UTF8
                               , (const unsigned char *)commonName.c_str()
                               , -1 // by strlen(email.c_str())
                               , -1 // loc, location
                               , 0))// set

    {
        char szCommonName [100] = "";
        int rc = X509_NAME_get_text_by_NID(name, NID_commonName, szCommonName, sizeof(szCommonName));
        (void )rc;
        rc = rc;
        char c = *szCommonName;
        c = c;
    }

    /*
     * Its self signed so set the issuer name to be the same as the subject.
     */
    X509_set_issuer_name(x, name);

    /* Add various extensions: standard extensions
    *  , can be prefix-ed by 'critical,'
    */
    AddExt(x, NID_basic_constraints, (char *)"CA:TRUE");
	
    /**
     * @brief add_ext(NID_key_usage)
     * RFC3280: [https://tools.ietf.org/html/rfc3280#section-4.2.1.3]
     * id-ce-keyUsage OBJECT IDENTIFIER ::=  { id-ce 15 }
     *     KeyUsage ::= BIT STRING {
     *     digitalSignature        (0),
     *     nonRepudiation          (1),
     *     keyEncipherment         (2),
     *     dataEncipherment        (3),
     *     keyAgreement            (4),
     *     keyCertSign             (5),
     *     cRLSign                 (6),
     *     encipherOnly            (7),
     *     decipherOnly            (8) }
     */
    AddExt(x, NID_key_usage, (char *)"digitalSignature"\
                                  ", nonRepudiation"\
                                  ", dataEncipherment"\
                                  ", keyAgreement"\
                                  ", keyCertSign"\
                                  ", cRLSign");

    /**
     * @brief add_ext(NID_ext_key_usage)
     * RFC3280: [https://tools.ietf.org/html/rfc3280#section-4.2.1.13]
     * id-ce-extKeyUsage OBJECT IDENTIFIER ::= { id-ce 37 }
     *     ExtKeyUsageSyntax ::= SEQUENCE SIZE (1..MAX) OF KeyPurposeId
     *     KeyPurposeId ::= OBJECT IDENTIFIER
     *
     *     anyExtendedKeyUsage OBJECT IDENTIFIER ::= { id-ce-extKeyUsage 0 }
     *     id-kp-serverAuth             OBJECT IDENTIFIER ::= { id-kp 1 }
     *     id-kp-clientAuth             OBJECT IDENTIFIER ::= { id-kp 2 }
     *     id-kp-codeSigning            OBJECT IDENTIFIER ::= { id-kp 3 }
     *     id-kp-emailProtection        OBJECT IDENTIFIER ::= { id-kp 4 }
     *     id-kp-timeStamping           OBJECT IDENTIFIER ::= { id-kp 8 }
     *     id-kp-OCSPSigning            OBJECT IDENTIFIER ::= { id-kp 9 }
     */
    AddExt(x, NID_ext_key_usage            
            , (char *)"anyExtendedKeyUsage"
           );

    AddExt(x, NID_subject_key_identifier
            , (char *)"hash");
			
    AddExt(x, NID_authority_key_identifier
            , (char *)"keyid:always");			

    /* Some Netscape specific extensions */
    AddExt(x, NID_netscape_cert_type
            , (char *)"emailCA, email, sslCA");

    AddExt(x, NID_subject_alt_name
            , (char *)(std::string("email:") + email).c_str());

    AddExt(x, NID_issuer_alt_name
            , (char *)"DNS:bitmail.bitbaba.com");

    AddExt(x, NID_netscape_comment
            , (char *)"DNS:bitmail.bitbaba.com");

    #ifdef CUSTOM_EXT
    /* Maybe even add our own extension based on existing */
    {
        int nid;
        nid = OBJ_create("1.2.3.4", "MyAlias", "My Test Alias Extension");
        X509V3_EXT_add_alias(nid, NID_netscape_comment);
        add_ext(x, nid, "example comment alias");
    }
    #endif

    if (!X509_sign(x, pk, EVP_sha1()))
        goto err_MakeCert;

    if (x){
        m_cert = CertToPem(x);
    }

    if (pk){
        m_key = PKeyToPem(pk, passphrase);
    }

    return (0);

 err_MakeCert:
    if (x) {
        X509_free(x);
        x = NULL;
    }
    if (pk) {
        EVP_PKEY_free(pk);
        pk = NULL;
    }

    return (1);
}

std::string CX509Cert::GetEmail() const
{
    X509 * x = GetCert();
    if (x == NULL){
        return "";
    }
    X509_NAME * name = X509_get_subject_name(x);
    if (name == NULL){
        return "";
    }
    char szEmail [100] = "";
    int rc = X509_NAME_get_text_by_NID(name, NID_pkcs9_emailAddress, szEmail, sizeof(szEmail));
    (void )rc;
    return szEmail;
}

/*
 * Add extension using V3 code: we can set the config file as NULL because we
 * wont reference any other sections.
 */
int CX509Cert::AddExt(X509 *cert, int nid, char *value)
{
    X509_EXTENSION *ex;
    X509V3_CTX ctx;
    /* This sets the 'context' of the extensions. */
    /* No configuration database */
    X509V3_set_ctx_nodb(&ctx);
    /*
     * Issuer and subject certs: both the target since it is self signed, no
     * request and no CRL
     */
    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
    if (!ex)
        return 1;

    X509_add_ext(cert, ex, -1);
    X509_EXTENSION_free(ex);
    return 0;
}

std::string CX509Cert::PKeyToPem(const EVP_PKEY * pkey, const std::string & passphrase)
{
    BIO* mbio = BIO_new(BIO_s_mem());// sink to memory.

    PEM_write_bio_RSAPrivateKey(mbio
                                , pkey->pkey.rsa
                                , EVP_des_ede3_cbc()
                                , NULL, 0
                                , NULL, (void*)passphrase.c_str());
    BUF_MEM * bufptr = NULL;
    BIO_get_mem_ptr(mbio, &bufptr);
    std::string sRsaPEMEncoded = "";
    sRsaPEMEncoded.append((char *)bufptr->data, bufptr->length);
    return sRsaPEMEncoded;
}

std::string CX509Cert::CertToPem(const X509 * cert)
{
    BIO* mbio = BIO_new(BIO_s_mem());// sink to memory.
    PEM_write_bio_X509(mbio, (X509*)cert);
    BUF_MEM * bufptr = NULL;
    BIO_get_mem_ptr(mbio, &bufptr);
    std::string sCertPemEncoded = "";
    sCertPemEncoded.append((char *)bufptr->data, bufptr->length);
    return sCertPemEncoded;
}

EVP_PKEY * CX509Cert::PemToPKey(const std::string & sRsa, const std::string & passphrase)
{
    BIO * mbio = BIO_new_mem_buf((void*)sRsa.c_str(), sRsa.length());
    if (!mbio) return NULL;

    RSA * rsa = PEM_read_bio_RSAPrivateKey(mbio
                                    , NULL /*or, &x */
                                    , NULL /*void callback*/
                                    , (void *)passphrase.c_str());
    if (!rsa) return NULL;

    EVP_PKEY * pkey = EVP_PKEY_new();
    if (!pkey) return NULL;

    if (!EVP_PKEY_assign_RSA(pkey, rsa))
        return NULL;

    return pkey;
}

X509 * CX509Cert::PemToCert(const std::string sCert)
{
    BIO * mbio = BIO_new_mem_buf((void*)sCert.c_str(), sCert.length());
    if (!mbio) return NULL;

    X509 * x = PEM_read_bio_X509(mbio
                                    , NULL /*or, &x */
                                    , NULL /*void callback*/
                                    , 0);

    return x;
}

X509 *  CX509Cert::GetCert() const
{
    return PemToCert(m_cert);
}

int CX509Cert::GetBits() const 
{
    return EVP_PKEY_bits(X509_get_pubkey(GetCert()));
}

std::string CX509Cert::GetPassphrase() const
{
    return m_passphrase;
}

std::string CX509Cert::GetCertByPem() const
{
    return m_cert;
}

EVP_PKEY *  CX509Cert::GetPrivateKey()
{
    if (m_key.empty()) return NULL;
    return PemToPKey(m_key, m_passphrase);
}

std::string CX509Cert::GetPrivateKeyAsEncryptedPem()
{
    return m_key;
}

/**
* Update passphrase
*/
int CX509Cert::SetPassphrase(const std::string & passphrase)
{
    if (m_passphrase == passphrase){
        return 0;
    }

    EVP_PKEY *pkey = GetPrivateKey();
    if (pkey == NULL){
        return 1;
    }

    std::string sKeyPem = PKeyToPem(pkey, passphrase);
    if (sKeyPem.empty()){
        return 2;
    }
    m_key = sKeyPem;
    m_passphrase = passphrase;
    return 0;
}

/* Simple S/MIME signing example */

std::string CX509Cert::Sign(const std::string & msg)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) return "";

    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out) return "";

    X509 *scert = NULL;
    EVP_PKEY *skey = NULL;
    CMS_ContentInfo *cms = NULL;
    int ret = 1, flags = 0;

    scert = GetCert();

    skey = GetPrivateKey();

    if (!scert || !skey)
        goto err;

    /* Sign content, MUST be detached, otherwise, it dose not work on mobile phone */

    /*
     * For simple S/MIME signing use PKCS7_DETACHED. On OpenSSL 0.9.9 only:
     * for streaming detached set PKCS7_DETACHED|PKCS7_STREAM for streaming
     * non-detached set PKCS7_STREAM
     */
    flags = (CMS_STREAM |CMS_NOSMIMECAP | CMS_CRLFEOL | CMS_TEXT | CMS_DETACHED );
    cms = CMS_sign(scert, skey, NULL, in, flags);

    if (!cms)
        goto err;

    if (!(flags & CMS_STREAM))
        BIO_reset(in);

    /* Write out S/MIME message */
    if (!SMIME_write_CMS(out, cms, in, flags))
        goto err;
    else{
        BUF_MEM * bufptr = NULL;
        BIO_get_mem_ptr(out, &bufptr);

        std::string p7out = "";
        p7out.append((char *)bufptr->data, bufptr->length);
        return p7out;
    }
 err:
    if (ret) {
        fprintf(stderr, "Error Signing Data\n");
        ERR_print_errors_fp(stderr);
    }
    CMS_ContentInfo_free(cms);
    X509_free(scert);
    EVP_PKEY_free(skey);
    BIO_free(in);
    BIO_free(out);

    return "";
}

/* Simple S/MIME verification example */

std::string CX509Cert::Verify(const std::string & msg)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) return "";

    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out) return "";

    BIO *cont = NULL;
    X509_STORE *st = NULL;
    X509 *cacert = NULL;
    CMS_ContentInfo *cms = NULL;

    int ret = 1;

    /* Set up trusted CA certificate store */
    st = X509_STORE_new();

    //HarryWu: here, CA cert is itself.
    cacert = GetCert();

    if (!cacert)
        goto err;

    if (!X509_STORE_add_cert(st, cacert))
        goto err;

    /* Sign content */
    cms = SMIME_read_CMS(in, &cont);

    if (!cms)
        goto err;

    if (!CMS_verify(cms, NULL/*signers' certs*/, st/*trusted CAs' cert*/, cont, out, 0)) {
        fprintf(stderr, "Verification Failure\n");
        goto err;
    }else{
        BUF_MEM * bufptr = NULL;
        BIO_get_mem_ptr(out, &bufptr);

        std::string p7out = "";
        p7out.append((char *)bufptr->data, bufptr->length);
        return p7out;
    }

    fprintf(stderr, "Verification Successful\n");

    ret = 0;

 err:
    if (ret) {
        fprintf(stderr, "Error Verifying Data\n");
        ERR_print_errors_fp(stderr);
    }
    CMS_ContentInfo_free(cms);
    X509_free(cacert);
    BIO_free(in);
    BIO_free(out);
    return "";
}

/**
 * Multiple Encrypting
 */
std::string CX509Cert::MEncrypt(const std::string & msg, const std::vector<CX509Cert> & certs)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) return "";

    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out) {
        BIO_free(in);
        return "";
    }

    X509 *rcert = NULL;
    STACK_OF(X509) *recips = NULL;
    CMS_ContentInfo *cms = NULL;
    int ret = 0;

    /*
     * On OpenSSL 0.9.9 only:
     * for streaming set PKCS7_STREAM
     */
    int flags = CMS_STREAM;

    /* Create recipient STACK and add recipient cert to it */
    recips = sk_X509_new_null();

    if ( !recips )
        goto err;

    for(std::vector<CX509Cert>::const_iterator it = certs.begin(); it != certs.end(); ++it){
        if (!sk_X509_push(recips, it->GetCert())){
            ret = 1;
            break;
        }
    }

    if (ret != 0)
        goto err;

    /*
     * sk_X509_pop_free will free up recipient STACK and its contents( the X509 * ptr)
     */

    /* encrypt content */
    cms = CMS_encrypt(recips, in, EVP_des_ede3_cbc(), flags);

    if (!cms)
        goto err;

    /* Write out S/MIME message */
    if (!SMIME_write_CMS(out, cms, in, flags))
        goto err;
    else{
        BUF_MEM * bufptr = NULL;
        BIO_get_mem_ptr(out, &bufptr);

        std::string p7out = "";
        p7out.append((char *)bufptr->data, bufptr->length);
        return p7out;
    }

 err:
    if (ret) {
        fprintf(stderr, "Error Encrypting Data\n");
        ERR_print_errors_fp(stderr);
    }
    CMS_ContentInfo_free(cms);
    sk_X509_pop_free(recips, X509_free);
    BIO_free(in);
    BIO_free(out);
    return "";

}

/* Simple S/MIME encrypt example */

std::string CX509Cert::Encrypt(const std::string & msg)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) return "";

    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out) {
        BIO_free(in);
        return "";
    }

    X509 *rcert = NULL;
    STACK_OF(X509) *recips = NULL;
    CMS_ContentInfo *cms = NULL;
    int ret = 1;

    /*
     * On OpenSSL 0.9.9 only:
     * for streaming set PKCS7_STREAM
     */
    int flags = CMS_STREAM;

    /* Read in recipient certificate */
    rcert = GetCert();

    if (!rcert)
        goto err;

    /* Create recipient STACK and add recipient cert to it */
    recips = sk_X509_new_null();

    if (!recips || !sk_X509_push(recips, rcert))
        goto err;

    /*
     * sk_X509_pop_free will free up recipient STACK and its contents so set
     * rcert to NULL so it isn't freed up twice.
     */
    rcert = NULL;

    /* encrypt content */
    cms = CMS_encrypt(recips, in, EVP_des_ede3_cbc(), flags);

    if (!cms)
        goto err;

    /* Write out S/MIME message */
    if (!SMIME_write_CMS(out, cms, in, flags))
        goto err;
    else{
        BUF_MEM * bufptr = NULL;
        BIO_get_mem_ptr(out, &bufptr);

        std::string p7out = "";
        p7out.append((char *)bufptr->data, bufptr->length);
        return p7out;
    }

    ret = 0;

 err:
    if (ret) {
        fprintf(stderr, "Error Encrypting Data\n");
        ERR_print_errors_fp(stderr);
    }
    CMS_ContentInfo_free(cms);
    X509_free(rcert);
    sk_X509_pop_free(recips, X509_free);
    BIO_free(in);
    BIO_free(out);
    return "";

}

/* Simple S/MIME Decrypt example */

std::string CX509Cert::Decrypt(const std::string & msg)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) return "";

    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out) return "";

    X509 *rcert = NULL;
    EVP_PKEY *rkey = NULL;
    CMS_ContentInfo * cms = NULL;
    int ret = 1;

    /* Read in recipient certificate and private key */

    rcert = GetCert();

    rkey = GetPrivateKey();

    if (!rcert || !rkey)
        goto err;

    /* Read content into PKCS7 */
    cms = SMIME_read_CMS(in, NULL);

    if (!cms)
        goto err;

    /* Decrypt S/MIME message */
    if (!CMS_decrypt(cms, rkey, rcert, NULL, out, 0)){
        {
            char errbuf [256] = "";
            unsigned long en = ERR_get_error();
            ERR_error_string(en, errbuf);
            std::string xerrstr = errbuf;
        }
        goto err;
    }
    else{
        BUF_MEM * bufptr = NULL;
        BIO_get_mem_ptr(out, &bufptr);

        std::string p7out = "";
        p7out.append((char *)bufptr->data, bufptr->length);
        return p7out;
    }

    ret = 0;

 err:
    if (ret) {
        fprintf(stderr, "Error Decrypting Data\n");
        ERR_print_errors_fp(stderr);
    }
    CMS_ContentInfo_free(cms);
    X509_free(rcert);
    EVP_PKEY_free(rkey);
    BIO_free(in);
    BIO_free(out);
    return "";
}

/* S/MIME signing example: 2 signers. OpenSSL 0.9.9 only */

std::string CX509Cert::MSign(const std::string & msg, const std::vector<CX509Cert> & singers)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) return "";

    BIO *out = NULL, *tbio = NULL;
    X509 *scert = NULL, *scert2 = NULL;
    EVP_PKEY *skey = NULL, *skey2 = NULL;
    CMS_ContentInfo *cms = NULL;
    int ret = 1;

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    tbio = BIO_new_file("signer.pem", "r");

    if (!tbio)
        goto err;

    scert = PEM_read_bio_X509(tbio, NULL, 0, NULL);

    BIO_reset(tbio);

    skey = PEM_read_bio_PrivateKey(tbio, NULL, 0, NULL);

    BIO_free(tbio);

    tbio = BIO_new_file("signer2.pem", "r");

    if (!tbio)
        goto err;

    scert2 = PEM_read_bio_X509(tbio, NULL, 0, NULL);

    BIO_reset(tbio);

    skey2 = PEM_read_bio_PrivateKey(tbio, NULL, 0, NULL);

    if (!scert2 || !skey2)
        goto err;

    in = BIO_new_file("sign.txt", "r");

    if (!in)
        goto err;

    cms = CMS_sign(NULL, NULL, NULL, in, CMS_STREAM | CMS_PARTIAL);

    if (!cms)
        goto err;

    /* Add each signer in turn */

    if (!CMS_add1_signer(cms, scert, skey, NULL, 0))
        goto err;

    if (!CMS_add1_signer(cms, scert2, skey2, NULL, 0))
        goto err;

    out = BIO_new_file("smout.txt", "w");
    if (!out)
        goto err;

    /* NB: content included and finalized by SMIME_write_PKCS7 */

    if (!SMIME_write_CMS(out, cms, in, CMS_STREAM))
        goto err;

    ret = 0;

 err:
    if (ret) {
        fprintf(stderr, "Error Signing Data\n");
        ERR_print_errors_fp(stderr);
    }
    CMS_ContentInfo_free(cms);
    X509_free(scert);
    EVP_PKEY_free(skey);
    X509_free(scert2);
    EVP_PKEY_free(skey2);
    BIO_free(in);
    BIO_free(out);
    BIO_free(tbio);
    return "";
}

std::string CX509Cert::GetCommonName() const
{
    X509 * x = GetCert();
    if (x == NULL){
        X509_free(x);
        return "";
    }
    X509_NAME * name = X509_get_subject_name(x);
    if (name == NULL){
        X509_free(x);
        return "";
    }
    char szCommonName [100] = "";
    int rc = X509_NAME_get_text_by_NID(name, NID_commonName, szCommonName, sizeof(szCommonName));
    (void )rc;
    X509_free(x);
    return szCommonName;
}

std::string CX509Cert::GetID() const
{
    X509 * x = GetCert();
    if (x == NULL){
        return "";
    }

    const unsigned int SHA1_DIGEST_LENTH = 20;
    const unsigned char BASE32TABLE[33] = "BM01346789ACDEFGHJKLNPQRSTUVWXYZ";
    const unsigned char HEXTABLE[33] = "0123456789ABCDEF";

    unsigned char sha1_hash[SHA1_DIGEST_LENTH];
    unsigned sha1_hash_len = SHA1_DIGEST_LENTH;
    /**
     * 1) X509_pubkey_digest produce [Subject key identifier]
     *    that is, sha1 hash of subject's public key structure,
     *    this hash is included in x509 certificate.
     *
     * 2) X509_digest produce [Thumbprint]
     *    that is, sha1 hash of all certificate,
     *    this hash is NOT included in x509 certificate.
     */
    int len = X509_digest(x, EVP_sha1(), sha1_hash, &sha1_hash_len);
    if (len != 1 || sha1_hash_len != SHA1_DIGEST_LENTH) {
        return "";
    }

    std::string result = "";
    for (unsigned int i = 0; i < SHA1_DIGEST_LENTH; i++){
        unsigned char c = sha1_hash[i];
        result += HEXTABLE[(c & 0xf0) >> 4];
        result += HEXTABLE[(c & 0x0f)];
    }

    X509_free(x);

    return result;
}

bool CX509Cert::IsValid() const
{
    if (this->m_cert.empty()){
        return false;
    }
    // TODO: more checks
    return true;
}

std::string CX509Cert::b64enc(const std::string & str)
{
    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out){
        return "";
    }

    BIO * b64 = BIO_new(BIO_f_base64());
    if (!b64){
        BIO_free(out);
        return "";
    }

    out = BIO_push(b64, out);

    BIO_write(out, (char *)str.c_str(), str.length());

    BIO_flush(out);

    BUF_MEM * bufptr = NULL;
    BIO_get_mem_ptr(out, &bufptr);
    std::string b64str = "";
    b64str.append((char *)bufptr->data, bufptr->length);

    BIO_free(b64);

    return b64str;
}

std::string CX509Cert::b64dec(const std::string & b64str)
{
    BIO * in = BIO_new_mem_buf((void*)b64str.c_str(), b64str.length());
    if (!in) return "";

    BIO * b64 = BIO_new(BIO_f_base64());
    if (!b64){
        BIO_free(in);
        return "";
    }

    in = BIO_push(b64, in);

    char buf [16] = "";
    int bufsz = sizeof(buf);

    std::string str = "";
    for(;;){
        int cb = BIO_read(in, buf, bufsz);
        if (cb <= 0){
            break;
        }
        str.append(buf, cb);
    }

    BIO_free(b64);

    return str;
}
