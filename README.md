# SentinelChat

SentinelChat is an end-to-end encrypted chat application that supports secure file transfer between clients using the Boost and CryptoPP libraries.

⚠️ **CAUTION**: The encryption configuration used in this application is for demonstration purposes ONLY! Using CBC mode in AES with a constant IV value is not recommended. Instead, use GCM mode, which provides AEAD (Authenticated Encryption with Associated Data).

## Security Considerations

While this application focuses on memory security, there are other potential attack vectors to consider:

- Man-in-the-Middle (MITM) / Interception attacks
- Message client_id spoofing / Client Impersonation / Authentication Bypass
- Denial of Service (DoS) attacks
- Replay vulnerabilities
- Protocol vulnerabilities
- Messaging Feature Denial of Service
- Possible Buffer Overflow vulnerabilities

For more information on secure software development, refer to the book "The Art of Software Security Assessment - Identifying and Preventing Software Vulnerabilities."

## Installation

### macOS

1. Install Xcode command-line tools:
```
xcode-select --install
```

2. Install Homebrew package manager:
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
3. Install required dependencies:
```
brew install boost cmake make sqlite
```

4. (Optional) Install the latest version of CryptoPP library using vcpkg:

```
vcpkg install cryptopp
```

Note: You can skip this step and use the pre-compiled libraries included in the client directory (both x86 and ARM versions are provided).

5. Build the client:

```
cd PATH/TO/CLIENT
make
```

6. Launch the server and clients in separate terminals:

```
Server
cd ../server/
python3 server.py

Client 1
cd client1/
./client

Client 2
cd client2/
./client
```

### Windows

You can follow similar steps as above using `vcpkg` or download the necessary libraries using Visual Studio `(NuGet)`.

## Usage

To establish a secure connection between two clients:

1. Register both clients.
2. Retrieve the list of clients on both clients.
3. Client A requests Client B's public key.
4. Client A requests Client B's symmetric key.
5. Client B requests pending messages.
6. Client B requests Client A's public key.
7. Client B sends their symmetric key.
8. Client A requests pending messages.
9. Client A sends their symmetric key.
10. Client B requests pending messages.

After completing these steps, you can start chatting and sending files securely between the clients.