SERVER_VER = 2

# requests (sizes)
UUID_SIZE = 16
VER_SIZE = 1
CODE_SIZE = 2
PAYLOAD_SIZE = 4
REQUEST_HEADER_SIZE = 23

# payload
USERNAME_SIZE = 255  # maximum name length
PUB_KEY_SIZE = 160  # maximum public key size

REQUEST_HEADER_FORMAT = '<16sBhI'  # <16sBBI

# request codes
REGISTER_USER = 1100  # server will ignore client's ID
GET_USERLIST = 1101  # payload field doesn't exist, payload_size = 0
GET_PUBLICKEY = 1102
SEND_MSG = 1103  # payload field doesn't exist, payload_size = 0
GET_PENDING_MSG = 1104
MSG_CONTENT_OFFSET = 21

MSG_TYPE_SIZE = 1  # size of message types
CONTENT_SIZE_SIZE = 4  # size of message content
MSG_SENT_PAYLOAD_SIZE = 20

# message types
GET_SYM_KEY = 1  # empty message content, content_size = 0.
SEND_SYM_KEY = 2  # message_content contains private symmetric key that's encrypted by the public key of dst client.
SEND_TXT_MSG = 3  # message_content contains encrypted text by symmetric key.
SEND_FILE = 4  # message_content contains encrypted file by symmetric key.

# responses
# The response header format consists of
# version - 1byte (B) | responseCode - 2bytes (H) | payloadSize - 4bytes (I) | content - blob
RESPONSE_HEADER_FORMAT = '<BHI'
REG_SUCC = 2100
GET_ULIST_SUCC = 2101
GET_PUB_KEY_SUCC = 2102
MSG_SENT_SUCC = 2103
GET_MSGS_SUCC = 2104
INTERNAL_ERROR = 9000
