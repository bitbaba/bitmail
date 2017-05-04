# include <bitmailcore/bitmail_internal.h>
#include <bitmailcore/x509cert.h>
/*
 * Certificate creation. Demonstrates some certificate related operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <openssl/pkcs12.h>

#if defined(X509_NAME) && (defined(_MSC_VER) || defined(MINGW_DDK_H))
#undef X509_NAME
#endif

CX509Cert::CX509Cert()
    : m_cert("")
    , m_key("")
    , m_passphrase("")
{

}

CX509Cert::CX509Cert(const std::string & certpem)
{
    X509 * x = PEM2Cert(certpem);
    if (x != NULL){
        m_cert = certpem;
        X509_free(x);
    }
}

CX509Cert::CX509Cert(const std::string & certpem, const std::string & privkey, const std::string & passphrase)
{
    ImportCert(certpem); ImportPrivKey(privkey, passphrase);
}


bool CX509Cert::Create(unsigned int bits
					, const std::string & commonName
					, const std::string & email
					, const std::string & passphrase)
{
    if (MakeCert(commonName, email, passphrase, bits)){
        return true;
    }
    m_passphrase = passphrase;
    return true;
}

bool CX509Cert::ImportCert(const std::string & sPemCert)
{
    X509 * x = PEM2Cert(sPemCert);
    if (x != NULL){
        m_cert = sPemCert;
        X509_free(x);
        return true;
    }
    return false;
}

bool CX509Cert::ImportPrivKey(const std::string &key, const std::string &passphrase)
{
    EVP_PKEY * privKey = PEM2PrivKey(key, passphrase);
    if (privKey != NULL){
        m_key = key; m_passphrase = passphrase;
        EVP_PKEY_free(privKey);
        return true;
    }
    return false;
}

/**
* http://linux.die.net/man/3/cms_verify
* https://www.openssl.org/docs/manmaster/crypto/CMS_verify.html
* CMS_get0_signers() retrieves the signing certificate(s) from cms, it must be called after a successful CMS_verify() operation.
* So, there are differences:
* http://linux.die.net/man/3/pkcs7_verify
* PKCS7_get0_signers() retrieves the signer's certificates from p7, it does not check their validity or whether any signatures are valid. The certs and flags parameters have the same meanings as in PKCS7_verify().
*/
bool CX509Cert::ImportCertFromSig(const std::string & msg)
{
    BIO * in = BIO_new_mem_buf((void*)msg.c_str(), msg.length());
    if (!in) {
        fprintf(stderr, "failed to create memory buffer\n");
        return false;
    }
    BIO *cont = NULL;
    PKCS7 * cms = NULL;
    STACK_OF(X509) * sk_signers = NULL;

    int ret = 1;
    int flags = 0;

    /* read signature */
    cms = SMIME_read_PKCS7(in, &cont);

    if (!cms){
        BIO_free(in);
        return false;
    }

    sk_signers = PKCS7_get0_signers(cms, NULL, 0);

    if (!sk_signers){
        PKCS7_free(cms);
        BIO_free(in);
        return false;
    }

    for (int i = 0; i < sk_X509_num(sk_signers); i++){
        X509 * x = sk_X509_value(sk_signers, i);
        m_cert = Cert2PEM(x);
        break;
    }

    PKCS7_free(cms);
    BIO_free(in);
    return true;
}

std::string CX509Cert::GetSigningTime(const std::string & sig)
{
    BIO * in = BIO_new_mem_buf((void*)sig.c_str(), sig.length());
    if (!in) return "";

    BIO *cont = NULL;
    CMS_ContentInfo * cms = NULL;

    /* read signature */
    cms = SMIME_read_CMS(in, &cont);

    if (!cms){ BIO_free(in); return "";}

    STACK_OF(CMS_SignerInfo) * sis = CMS_get0_SignerInfos(cms);

    if (sk_CMS_SignerInfo_num(sis) == 0){
        CMS_ContentInfo_free(cms);
        BIO_free(in);
        return "";
    }

    /* First signer */
    CMS_SignerInfo *si = sk_CMS_SignerInfo_value(sis, 0);
    if (si == NULL){
        CMS_ContentInfo_free(cms);
        BIO_free(in);
        return "";
    }

    X509_ATTRIBUTE *xa = CMS_signed_get_attr(si, CMS_signed_get_attr_by_NID(si, NID_pkcs9_signingTime, -1));
    if (xa == NULL){
        CMS_ContentInfo_free(cms);
        BIO_free(in);
        return "";
    }

    /* First SigningTime attribute */
    ASN1_TYPE *so = sk_ASN1_TYPE_value(xa->value.set, 0);
    if (so == NULL){
        CMS_ContentInfo_free(cms);
        BIO_free(in);
        return "";
    }

    std::string sigtime = "";
    switch (so->type) {
            case V_ASN1_UTCTIME:
            sigtime.append("UTC:");
            sigtime.append((char *)so->value.utctime->data, so->value.utctime->length);
            break;
            case  V_ASN1_GENERALIZEDTIME:
            sigtime.append("GT:");
            sigtime.append((char *)so->value.generalizedtime->data, so->value.generalizedtime->length);
            break;
    }

    CMS_ContentInfo_free(cms);
    BIO_free(in);
    return sigtime;
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

    /** E **/
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

    /** CN **/
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

    /** O **/
    if (X509_NAME_add_entry_by_txt(name, "organizationName",
                               MBSTRING_UTF8
                               , (const unsigned char *)"Bitbaba corporation"
                               , -1 // by strlen(email.c_str())
                               , -1 // loc, location
                               , 0))// set

    {
        char szCommonName [100] = "";
        int rc = X509_NAME_get_text_by_NID(name, NID_organizationName, szCommonName, sizeof(szCommonName));
        (void )rc;
        rc = rc;
        char c = *szCommonName;
        c = c;
    }

    /** OU **/
    if (X509_NAME_add_entry_by_txt(name, "organizationalUnitName",
                               MBSTRING_UTF8
                               , (const unsigned char *)"bitmail department of Bitbaba corporation"
                               , -1 // by strlen(email.c_str())
                               , -1 // loc, location
                               , 0))// set

    {
        char szCommonName [100] = "";
        int rc = X509_NAME_get_text_by_NID(name, NID_organizationalUnitName, szCommonName, sizeof(szCommonName));
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

    /* Some Netscape specific extensions see `man x509_config'*/
    AddExt(x, NID_netscape_cert_type
            , (char *)"client, server, email, objsign, sslCA, emailCA, objCA");

    AddExt(x, NID_subject_alt_name
            , (char *)(std::string("email:") + email).c_str());

    AddExt(x, NID_issuer_alt_name
            , (char *)(std::string("email:") + email).c_str());

    AddExt(x, NID_netscape_comment
            , (char *)(std::string("email:") + email).c_str());

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
        m_cert = Cert2PEM(x);
        X509_free(x); x = NULL;
    }

    if (pk){
        m_key = PrivKey2PEM(pk, passphrase);
        EVP_PKEY_free(pk); pk = NULL;
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

    /** Note: diff between 'Builtin object token' vs 'Software security device'
     * https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/Tools/certutil
     * https://security.stackexchange.com/questions/144312/mozilla-firefox-builtin-object-token-vs-software-security-device/144454
     * (1) Builtin Object Tokens are root certificates in the default Network Security Services (NSS) database as installed on the user's PC when the user installed the software (e.g., Firefox) that uses them.
     * (2) A BuiltIn Object Token will continue to be [a Builtin Object Token], even if the user changes the trust bits.
     * (3) A Software Security Device can hold all kinds of certificates, not just root certificates.
     * (4) For the Authorities list in the Certificate Manager, the certs that are labeled "Software Security Device" are root and intermediate certs that have been imported (e.g. not in the default list). The intermediate certs are often automatically imported, such as when you visit a website with an SSL cert signed by an intermediate CA.
     *
     * https://dxr.mozilla.org/mozilla/source/security/nss/cmd/certutil/certutil.c
     * http://certificate.fyicenter.com/389_Mozilla_CertUtil_Download_Mozilla_certutil_Tool_for_Window.html
     * ftp://ftp.mozilla.org/pub/mozilla.org/security/nss/releases
     * https://www.mozilla.org/en-US/about/governance/policies/security-group/certs/policy/
     * http://kb.mozillazine.org/Installing_an_SMIME_certificate
     */

    return (1);
}

std::string CX509Cert::GetEmail() const
{
    X509 * x = PEM2Cert(m_cert);
    if (x == NULL){
        return "";
    }
    X509_NAME * name = X509_get_subject_name(x);
    if (name == NULL){
    	X509_free(x);
        return "";
    }
    char szEmail [100] = "";
    X509_NAME_get_text_by_NID(name, NID_pkcs9_emailAddress, szEmail, sizeof(szEmail));
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

unsigned int CX509Cert::GetBits() const
{
	X509 * x = PEM2Cert(m_cert);
    unsigned int bits = EVP_PKEY_bits(X509_get_pubkey(x));
    X509_free(x);
    return bits;
}

std::string CX509Cert::GetPassphrase() const
{
    return m_passphrase;
}

std::string CX509Cert::ExportCert() const
{
    return m_cert;
}

std::string CX509Cert::ExportPrivKey() const
{
    return m_key;
}

std::string CX509Cert::ExportPKCS12() const
{
    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out) {
        return "";
    }
    PKCS12 *p12 = NULL;

    X509 *ucert = PEM2Cert(m_cert);
    EVP_PKEY *key = PEM2PrivKey(m_key, m_passphrase);

    // `enc' has no use in `export' mode
    p12 = PKCS12_create(const_cast<char *>(m_passphrase.c_str())
                        , const_cast<char*>(GetCommonName().c_str())
                        , key
                        , ucert
                        , NULL
                        , NID_pbe_WithSHA1And3_Key_TripleDES_CBC
                        , NID_pbe_WithSHA1And40BitRC2_CBC
                        , PKCS12_DEFAULT_ITER
                        , -1
                        , KEY_EX);

    if (!p12){
        EVP_PKEY_free(key);
        X509_free(ucert);
        return "";
    }

    PKCS12_set_mac(p12
                   , const_cast<char *>(m_passphrase.c_str())
                   , -1
                   , NULL
                   , 0
                   , PKCS12_DEFAULT_ITER
                   , NULL);

    if (i2d_PKCS12_bio(out, p12)){
        BUF_MEM * bufptr = NULL;
        BIO_get_mem_ptr(out, &bufptr);
        std::string p7out = "";
        p7out.append((char *)bufptr->data, bufptr->length);
        EVP_PKEY_free(key);
        X509_free(ucert);
        PKCS12_free(p12);
        return p7out;
    }

    EVP_PKEY_free(key);
    X509_free(ucert);
    PKCS12_free(p12);
    return "";
}

/**
* Update passphrase
*/
bool CX509Cert::SetPassphrase(const std::string & passphrase)
{
    if (passphrase.empty()){
        return false;
    }

    if (m_passphrase == passphrase){
        return true;
    }

    EVP_PKEY *pkey = PEM2PrivKey(m_key, m_passphrase);
    if (pkey == NULL){
        return false;
    }

    m_key = PrivKey2PEM(pkey, passphrase);
    m_passphrase = passphrase;
    EVP_PKEY_free(pkey);
    return true;
}

/* Simple S/MIME signing example */

std::string CX509Cert::Sign(const std::string & msg)
{
    BIO * in = BIO_new_mem_buf((void*)msg.data(), msg.length());
    if (!in) return "";

    BIO* out = BIO_new(BIO_s_mem());// sink to memory.
    if (!out) {
    	BIO_free(in);
    	return "";
    }

    X509 *scert = NULL;
    EVP_PKEY *skey = NULL;
    CMS_ContentInfo *cms = NULL;
    int ret = 1, flags = 0;

    scert = PEM2Cert(m_cert);
    if (!scert){
    	BIO_free(in);
    	BIO_free(out);
    	return "";
    }

    skey = PEM2PrivKey(m_key, m_passphrase);
    if (!skey){
    	BIO_free(in);
    	BIO_free(out);
    	X509_free(scert);
    	return "";
    }

    /* Sign content, MUST be detached, otherwise, it dose not work on mobile phone */

    /*
     * For simple S/MIME signing use PKCS7_DETACHED. On OpenSSL 0.9.9 only:
     * for streaming detached set PKCS7_DETACHED|PKCS7_STREAM for streaming
     * non-detached set PKCS7_STREAM
     */
    /**
     * if we try to compose mime data by ourself, use the `CMS_BINARY' flag instead of
     * the flag `CMS_TEXT'.
     * and remeber to use `CMS_DETACHED' flag always.
     * NO need for CMS_TEXT, and CMS_BINARY, Just left them two absent.
     */
    flags = (CMS_STREAM |CMS_NOSMIMECAP | CMS_CRLFEOL | CMS_DETACHED );
    cms = CMS_sign(scert, skey, NULL, in, flags);

    if (!cms){
    	BIO_free(in);
    	BIO_free(out);
    	X509_free(scert);
    	EVP_PKEY_free(skey);
    	return "";
    }

    if (!(flags & CMS_STREAM))
        BIO_reset(in);

    /* Write out S/MIME message */
    if (!SMIME_write_CMS(out, cms, in, flags)){
    	BIO_free(in);
    	BIO_free(out);
    	X509_free(scert);
    	EVP_PKEY_free(skey);
        CMS_ContentInfo_free(cms);
    	return "";
    }
    else{
        BUF_MEM * bufptr = NULL;
        BIO_get_mem_ptr(out, &bufptr);
        std::string p7out = "";
        p7out.append((char *)bufptr->data, bufptr->length);
        return p7out;
    }
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
    cacert = PEM2Cert(m_cert);

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
        if (!sk_X509_push(recips, PEM2Cert(it->m_cert))){
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
    rcert = PEM2Cert(m_cert);

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

    rcert = PEM2Cert(m_cert);

    rkey = PEM2PrivKey(m_key, m_passphrase);

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
    X509 * x = PEM2Cert(m_cert);
    if (x == NULL){
        return "";
    }
    X509_NAME * name = X509_get_subject_name(x);
    if (name == NULL){
        X509_free(x);
        return "";
    }
    char szCommonName [100] = "";
    X509_NAME_get_text_by_NID(name, NID_commonName, szCommonName, sizeof(szCommonName));
    X509_free(x);
    return szCommonName;
}

std::string CX509Cert::GetID() const
{
    X509 * x = PEM2Cert(m_cert);
    if (x == NULL){
        return "";
    }

    unsigned char sha256_hash[EVP_MAX_MD_SIZE];
    unsigned sha256_hash_len = EVP_MAX_MD_SIZE;
    /**
     * 1) X509_pubkey_digest produce [Subject key identifier]
     *    that is, sha1 hash of subject's public key structure,
     *    this hash is included in x509 certificate.
     *
     * 2) X509_digest produce [Thumbprint]
     *    that is, sha1 hash of all certificate,
     *    this hash is NOT included in x509 certificate.
     */
    int len = X509_digest(x, EVP_sha256(), sha256_hash, &sha256_hash_len);
    if (len != 1) {
        X509_free(x);
        return "";
    }

    std::string result;
    result.append((char *)sha256_hash, sha256_hash_len);
    result = b64enc(result, false);

    X509_free(x);

    return result;
}

bool CX509Cert::IsValid() const
{
    return !m_cert.empty();
}

std::string CX509Cert::hash(const std::string & str, const std::string & algo)
{
    EVP_MD_CTX mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    int md_len, i;

    md = EVP_get_digestbyname(algo.c_str());

    if(!md) {
        return "";
    }

    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);
    EVP_DigestUpdate(&mdctx, str.data(), str.length());
    EVP_DigestFinal_ex(&mdctx, md_value, (unsigned int *) &md_len);
    EVP_MD_CTX_cleanup(&mdctx);

    std::string h;
    for(i = 0; i < md_len; i++){
        char buf [16] = "";
        sprintf(buf, "%02x", md_value[i]);
        h += buf;
    }
    return h;
}

std::string CX509Cert::b64enc(const std::string & str, bool crlf)
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

    if (!crlf){
        BIO_set_flags(out, BIO_FLAGS_BASE64_NO_NL);
    }

    BIO_write(out, (char *)str.c_str(), str.length());

    BIO_flush(out);

    BUF_MEM * bufptr = NULL;
    BIO_get_mem_ptr(out, &bufptr);
    std::string b64str = "";
    b64str.append((char *)bufptr->data, bufptr->length);

    BIO_free(b64);

    return b64str;
}

std::string CX509Cert::b64dec(const std::string & b64str, bool crlf)
{
    /**
     * @TODO: if no tail with CRLF, this will fail?
     */
    BIO * in = BIO_new_mem_buf((void*)b64str.c_str(), b64str.length());
    if (!in) return "";

    BIO * b64 = BIO_new(BIO_f_base64());
    if (!b64){
        BIO_free(in);
        return "";
    }

    in = BIO_push(b64, in);

    if (!crlf){
        BIO_set_flags(in, BIO_FLAGS_BASE64_NO_NL);
    }

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

/*Public Key*/
std::string CX509Cert::PKSign(const std::string & msg, const std::string & dgst_algo)
{
    int rv = 0;
    EVP_PKEY_CTX * ctx = NULL;
    EVP_PKEY * pkey = PEM2PrivKey(m_key, m_passphrase);
    ctx = EVP_PKEY_CTX_new(pkey, NULL);
    EVP_PKEY_sign_init(ctx);

    std::string dgst = this->hash(msg, dgst_algo);

    size_t outlen = 0;
    unsigned char * out = NULL;
    // caculate the required size for output buffer.
    rv = EVP_PKEY_sign(ctx, out, &outlen, (const unsigned char *)dgst.data(), dgst.length());
    // alocate buffer
    out = (unsigned char *)malloc(outlen);
    // do real sign
    rv = EVP_PKEY_sign(ctx, out, &outlen, (const unsigned char *)dgst.data(), dgst.length());
    // free buf
    free(out);
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return msg;
}

std::string CX509Cert::PKVerify(const std::string & msg, const std::string & dgst, const std::string & sig)
{
    return "";
}

std::string CX509Cert::PKEncrypt(const std::string & msg)
{
    return "";
}

std::string CX509Cert::PKDecrypt(const std::string & code)
{
    return "";
}

/*Symmetric Key*/
std::string CX509Cert::SKEncrypt(const std::string & msg, const std::string & algo, const std::string & secret)
{
    return "";
}

std::string CX509Cert::SKDecrypt(const std::string & code, const std::string & algo, const std::string & secret)
{
    return "";
}

/** PEM Utililties **/
std::string CX509Cert::PrivKey2PEM(const EVP_PKEY * pkey, const std::string & passphrase)
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

std::string CX509Cert::Cert2PEM(const X509 * cert)
{
    BIO* mbio = BIO_new(BIO_s_mem());// sink to memory.
    PEM_write_bio_X509(mbio, (X509*)cert);
    BUF_MEM * bufptr = NULL;
    BIO_get_mem_ptr(mbio, &bufptr);
    std::string sCertPemEncoded = "";
    sCertPemEncoded.append((char *)bufptr->data, bufptr->length);
    return sCertPemEncoded;
}

EVP_PKEY * CX509Cert::PEM2PrivKey(const std::string & sRsa, const std::string & passphrase)
{
    BIO * mbio = BIO_new_mem_buf((void*)sRsa.c_str(), sRsa.length());
    if (!mbio) return NULL;

    RSA * rsa = PEM_read_bio_RSAPrivateKey(mbio
                                    , NULL /*or, &x */
                                    , NULL /*void callback*/
                                    , (void *)passphrase.c_str());
    if (!rsa) {
        BIO_free(mbio);mbio = NULL;
        return NULL;
    }

    EVP_PKEY * pkey = EVP_PKEY_new();
    if (!pkey) {
        BIO_free(mbio);mbio = NULL;
        return NULL;
    }

    if (!EVP_PKEY_assign_RSA(pkey, rsa)){
        BIO_free(mbio);mbio = NULL;
        EVP_PKEY_free(pkey); pkey = NULL;
        return NULL;
    }

    return pkey;
}

X509 * CX509Cert::PEM2Cert(const std::string sCert)
{
    BIO * mbio = BIO_new_mem_buf((void*)sCert.c_str(), sCert.length());
    if (!mbio) return NULL;

    X509 * x = PEM_read_bio_X509(mbio
                                    , NULL /*or, &x */
                                    , NULL /*void callback*/
                                    , 0);

    BIO_free(mbio);

    return x;
}
