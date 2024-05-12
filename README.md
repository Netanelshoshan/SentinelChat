# SentinelChat

[![SentinelChat CI](https://github.com/netanelshoshan/SentinelChat/workflows/SentinelChat%20CI/badge.svg)](https://github.com/netanelshoshan/SentinelChat/actions/workflows/ci.yml) [![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

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

```bash
# Check out the library.
git clone https://github.com/Netanelshoshan/SentinelChat.git
# Build the project
make
# Go to the server directory and run it.
python3 server/server.py
# In a seperate terminal, run the two clients
cd client1/ && ./client
cd client2/ && ./client
```

```
Follow the steps below to establish a secure connection between two clients:

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
```

## Architecture

- The application is client-server based, meaning,
  the messages are relayed through the server.

- The clients will send a "pull request" to the server to check for new messages and server will then send the messages to the clients.

- This will allow the clients to send messages while both online and offline.
  In addition, the messages will be encrypted on the client side and are sent in an encrypted form to the server,
  so even the server will not be able to read the messages, only the recipient client will be able to decrypt the messages.

### Under The Hood:

### Server

The server is written in Python and will act in a stateless manner,
meaning each request will be handled independently and the server will not store any state information about the clients.

The db will hold two tables, one for the clients and one for the messages.

### Clients Table

| Name      | Type              | Details                                                   |
|-----------|-------------------|-----------------------------------------------------------|
| ID        | 16 bytes          | uuid                                                      |
| userName  | string- 255 bytes | ascii representing user name - **null terminated**        |
| publicKey | 160 bytes         | client's public key                                       |
| lastSeen  | date and time     | The time the server got it's last request from the client |


### Messages Table

| Name       | Type     | Details          |
|------------|----------|------------------|
| ID         | 4 bytes  | index            |
| toClient   | 15 bytes | recipient's uuid |
| fromClient | 16 bytes | sender's uuid    |
| type       | byte     | type of message  |
| content    | blob     | message content  |

### Protocol Details

#### Requests
| **Request** | field name                                                                                                                    | size                                                                                                                    | details                                                                                                                                                                                                     |
|---------|-------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Header  | Client ID <br/> ----------------- <br/> Version <br/> ----------------- <br/> Code <br/> ----------------- <br/> Payload size | 16 bytes <br/> ----------------- <br/> byte <br/> ----------------- <br/> 2 bytes <br/> ----------------- <br/> 4 bytes | Client's uuid <br/> ------------------------------------ <br/> Client's version <br/> ------------------------------------ <br/> Request code <br/> ------------------------------------ <br/> Request size |
| Payload | Payload                                                                                                                       | Changes                                                                                                                 | Request content.<br/> Changes according<br/> to the type of request                                                                                                                                         |

#### Responses

| **Response** | field name                                                                                                          | size                                                                              | details                                                                                                                                        |
|----------|---------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| Header   | Version <br/> ----------------- <br/> Code <br/> ----------------- <br/> Payload size  | byte <br/> ----------------- <br/> 2 bytes <br/> ----------------- <br/> 4 bytes  | server's version <br/> ------------------------------------ <br/> response code <br/> ------------------------------------ <br/> response size |
| Payload  | Payload                                                                                                             | Changes                                                                           | Response content.<br/> Changes according<br/> to the type of response                                                                          |

