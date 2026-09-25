#include "ServerCert.hpp"

namespace NetworkMiner {
namespace PacketStream {

ServerCert* ServerCert::instance() {
    static ServerCert inst;
    return &inst;
}

ServerCert::ServerCert() {
    /*
     * Password: netresec
     * SSL X509 certificate created like this:
     * openssl genrsa -des3 -out rsaPriv.pem 1024
     * openssl req -new -x509 -key rsaPriv.pem -out cacert.pem -days 4000
     * openssl pkcs12 -export -in cacert.pem -out netresec.p12 -name "Netresec" -inkey rsaPriv.pem
     * base64 netresec.p12 (or open in notepad++ and run plugins/MIME/Base64 Encode)
     * 
     * Note: In Qt, we extracted the certificate and private key from the base64-encoded PKCS#12
     * back to PEM format to avoid issues with OpenSSL 3.0 dropping legacy PKCS#12 support by default.
     */

    QByteArray certData = 
        "-----BEGIN CERTIFICATE-----\n"
        "MIICQDCCAamgAwIBAgIJAP8fBnxr+CflMA0GCSqGSIb3DQEBBQUAMCAxCzAJBgNV\n"
        "BAYTAlNFMREwDwYDVQQKEwhOZXRyZXNlYzAeFw0xMTA5MDEyMTE3MDBaFw0yMjA4\n"
        "MTQyMTE3MDBaMCAxCzAJBgNVBAYTAlNFMREwDwYDVQQKEwhOZXRyZXNlYzCBnzAN\n"
        "BgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAvzy+H8YTQNbFfg44I7Yq5A87OYknsdXP\n"
        "YWipBsxcvmzfmGNsSdRFU6x8/+CaFg4Rhr3rL+3Ga5x9ZZnr2mkWiF/WhfDm7Tv/\n"
        "ghVif0RlAvnjlbRKTiExysnwvmXY1v0chPH3guTAIjWsFB94iCj0iMlmusdLTiuV\n"
        "R+nJKt1T3WUCAwEAAaOBgTB/MB0GA1UdDgQWBBRiUhFLfVUs57UFUD3usE08XyuA\n"
        "mDBQBgNVHSMESTBHgBRiUhFLfVUs57UFUD3usE08XyuAmKEkpCIwIDELMAkGA1UE\n"
        "BhMCU0UxETAPBgNVBAoTCE5ldHJlc2VjggkA/x8GfGv4J+UwDAYDVR0TBAUwAwEB\n"
        "/zANBgkqhkiG9w0BAQUFAAOBgQAVTdkvxqXXWaYCaEfm+MxmMjCTs/tLj+Tgs6/f\n"
        "DwkxcjoZ0zdQ+Zhxx0bigr4vQ61IvQPIasXpfKkkcPzyESZhK9CmKbzvo2Felgrv\n"
        "xmDltBXOWkEXYHt9LZ87Yz63E2KSYcTQqSdM6ZVq4ZXJFjRQwxOyD7m2NIIm/8rN\n"
        "DxGwzg==\n"
        "-----END CERTIFICATE-----\n";

    QByteArray keyData = 
        "-----BEGIN PRIVATE KEY-----\n"
        "MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAL88vh/GE0DWxX4O\n"
        "OCO2KuQPOzmJJ7HVz2FoqQbMXL5s35hjbEnURVOsfP/gmhYOEYa96y/txmucfWWZ\n"
        "69ppFohf1oXw5u07/4IVYn9EZQL545W0Sk4hMcrJ8L5l2Nb9HITx94LkwCI1rBQf\n"
        "eIgo9IjJZrrHS04rlUfpySrdU91lAgMBAAECgYA6Y6eBOA8k2u6N4HWPoc1SuLvS\n"
        "g2gVSjz5duU06CzhD4jtlF255RANzjqUPdWOKll7lH08sxLERJGc9fQnSbLRBUFZ\n"
        "mbHTvfTKwDH5ulWt2IoHo8u7Qw2ax99fEbdmchmu2J4mTTUyCvx7nztu7lhTmBKy\n"
        "jS1Bqr/5QOr/B719eQJBAO6scksIRpqvbgbppshK2gwy3YECs3Ko1CmZc3/IghiC\n"
        "pMj3KWEjWdemsLkhPOtVyhd4yhWHCD2/JSIriOpUzf8CQQDNHrsNOUm5yv2AZxBC\n"
        "y1/jEr3okrb4n922CXhGxwWubmqb0vI6Dj72LnKt0XH0/M3jg4PZGMBWkcrZx4uN\n"
        "DNybAkBzmEe+uWJgtsiGRpHFfiM/Nz96pVE+HjCreSusXvNLZbLjtnlvF40C2jPL\n"
        "ZiK4JBO97H2LkZvXNAWLGCS2wZ4HAkB61WJSgvIBjuvMDuwP89ZSFxEey21C6glN\n"
        "T9J1IA3o7TpuYh6oYYuWxS3rpTdG7HUnP8SfJ1FXBiuhyvto1kDtAkEAyURZI/Oz\n"
        "vDsjtJer/HMIWErcGR+c6DCUvuIYF0hAE+cRDbkLlv76GFniac4OI+i/WWbTdjMt\n"
        "qr4WG+RCP6pRmg==\n"
        "-----END PRIVATE KEY-----\n";

    auto certs = QSslCertificate::fromData(certData);
    if (!certs.isEmpty()) {
        m_certificate = certs.first();
    }
    
    m_privateKey = QSslKey(keyData, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
}

QSslCertificate ServerCert::certificate() const {
    return m_certificate;
}

QSslKey ServerCert::privateKey() const {
    return m_privateKey;
}

} // namespace PacketStream
} // namespace NetworkMiner
