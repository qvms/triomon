#pragma once

namespace PacketParser {
    class NetworkCredential;
namespace Events {

class CredentialEventArgs {
public:
    NetworkCredential* Credential;
    CredentialEventArgs(NetworkCredential* credential) : Credential(credential) {}
};

}
}