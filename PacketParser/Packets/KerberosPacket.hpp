#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>
#include <tuple>
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

class KerberosPacket : public AbstractPacket {
public:
    enum class MessageType : uint8_t {
        krb_null = 0,
        krb_as_req = 10,
        krb_as_rep = 11,
        krb_tgs_req = 12,
        krb_tgs_rep = 13,
        krb_ap_req = 14,
        krb_ap_rep = 15,
        krb_safe = 20,
        krb_priv = 21,
        krb_cred = 22,
        krb_error = 30
    };

    enum class NameType : int {
        KRB5_NT_UNKNOWN = 0,
        KRB5_NT_PRINCIPAL = 1,
        KRB5_NT_SRV_INST = 2,
        KRB5_NT_SRV_HST = 3,
        KRB5_NT_SRV_XHST = 4,
        KRB5_NT_UID = 5,
        KRB5_NT_X500_PRINCIPAL = 6,
        KRB5_NT_SMTP_NAME = 7,
        KRB5_NT_ENTERPRISE_PRINCIPAL = 10,
        KRB5_NT_WELLKNOWN = 11,
        KRB5_NT_ENT_PRINCIPAL_AND_ID = -130,
        KRB5_NT_MS_PRINCIPAL = -128,
        KRB5_NT_MS_PRINCIPAL_AND_ID = -129,
        KRB5_NT_NTLM = -1200
    };

    enum class PADataType : uint16_t {
        NONE = 0,
        TGS_REQ = 1,
        AP_REQ = 1,
        ENC_TIMESTAMP = 2,
        PW_SALT = 3,
        ENC_UNIX_TIME = 5,
        SANDIA_SECUREID = 6,
        SESAME = 7,
        OSF_DCE = 8,
        CYBERSAFE_SECUREID = 9,
        AFS3_SALT = 10,
        ETYPE_INFO = 11,
        SAM_CHALLENGE = 12,
        SAM_RESPONSE = 13,
        PK_AS_REQ_19 = 14,
        PK_AS_REP_19 = 15,
        PK_AS_REQ_WIN = 15,
        PK_AS_REQ = 16,
        PK_AS_REP = 17,
        PA_PK_OCSP_RESPONSE = 18,
        ETYPE_INFO2 = 19,
        USE_SPECIFIED_KVNO = 20,
        SVR_REFERRAL_INFO = 20,
        SAM_REDIRECT = 21,
        GET_FROM_TYPED_DATA = 22,
        SAM_ETYPE_INFO = 23,
        SERVER_REFERRAL = 25,
        ALT_PRINC = 24,
        SAM_CHALLENGE2 = 30,
        SAM_RESPONSE2 = 31,
        EXTRA_TGT = 41,
        TD_KRB_PRINCIPAL = 102,
        PK_TD_TRUSTED_CERTIFIERS = 104,
        PK_TD_CERTIFICATE_INDEX = 105,
        TD_APP_DEFINED_ERROR = 106,
        TD_REQ_NONCE = 107,
        TD_REQ_SEQ = 108,
        PA_PAC_REQUEST = 128,
        FOR_USER = 129,
        FOR_X509_USER = 130,
        FOR_CHECK_DUPS = 131,
        AS_CHECKSUM = 132,
        PK_AS_09_BINDING = 132,
        CLIENT_CANONICALIZED = 133,
        FX_COOKIE = 133,
        AUTHENTICATION_SET = 134,
        AUTH_SET_SELECTED = 135,
        FX_FAST = 136,
        FX_ERROR = 137,
        ENCRYPTED_CHALLENGE = 138,
        OTP_CHALLENGE = 141,
        OTP_REQUEST = 142,
        OTP_CONFIRM = 143,
        OTP_PIN_CHANGE = 144,
        EPAK_AS_REQ = 145,
        EPAK_AS_REP = 146,
        PKINIT_KX = 147,
        PKU2U_NAME = 148,
        REQ_ENC_PA_REP = 149,
        SUPPORTED_ETYPES = 165
    };

private:
    MessageType m_msgType;
    std::vector<std::tuple<QString, int, std::vector<uint8_t>>> m_asnData;

public:
    KerberosPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool packetHasLenghtFieldHeader);

    MessageType MsgType() const { return m_msgType; }
    bool IsRequest() const;
    const std::vector<std::tuple<QString, int, std::vector<uint8_t>>>& AsnData() const { return m_asnData; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
