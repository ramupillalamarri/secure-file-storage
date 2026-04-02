# Secure File Storage Server - Project Summary

## Project Status: ✅ SUCCESSFULLY BUILT AND RUNNING

The Secure File Storage Server project has been successfully created, compiled, and is now running on your system.

---

## What Was Built

A **client-server file storage system** in C with the following features:

### Core Features
1. **User Authentication** 
   - User registration with username and password
   - User login verification
   - Session management (login/logout)

2. **File Operations**
   - Upload files to the server
   - Download files from the server
   - List all user files
   - Delete files

3. **Multi-Client Support**
   - Server accepts multiple client connections **concurrently** using Windows threads
   - Each client session runs in its own thread
   - User-specific file storage with thread-safe authentication

4. **File Storage Architecture**
   - Each user has their own `storage/username` directory
   - Files are stored unencrypted (simplified demo version)
   - User credentials stored in `users.dat`

---

## How to Use

### Terminal 1: Start the Server
```bash
cd "c:\ramu\project\CN- project"
server\server.exe
```

You should see:
```
Server listening on port 8080
```

### Terminal 2: Run the Client
```bash
cd "c:\ramu\project\CN- project"
client\client.exe
```

You should see:
```
Secure File Storage Client
Commands: register <user> <pass>, login <user> <pass>, upload <file>, download <file>, list, delete <file>, logout, quit
>
```

### Example Commands

**1. Register a new user:**
```
> register alice password123
Response: OK
```

**2. Login:**
```
> login alice password123
Logged in
```

**3. Upload a file:**
```
> upload testfile.txt
Response: OK
```

**4. List files:**
```
> list
Files:
testfile.txt
```

**5. Download a file:**
```
> download testfile.txt
Downloaded testfile.txt
```

**6. Delete a file:**
```
> delete testfile.txt
Response: OK
```

**7. Logout:**
```
> logout
Logged out
```

**8. Exit:**
```
> quit
```

---

## Project Structure

```
CN- project/
├── server/
│   ├── server.exe           # Server executable (compiled)
│   ├── server.c             # Main server logic
│   ├── auth.c/auth.h        # User authentication functions
│   ├── crypto.c/crypto.h    # File transfer and serialization
│   ├── file_ops.c/file_ops.h # File operations (upload/download/delete)
│   └── common.h             # Shared definitions
├── client/
│   ├── client.exe           # Client executable (compiled)
│   ├── client.c             # Interactive command-line client
│   └── common.h             # Shared definitions
├── storage/                 # User file storage (created at runtime)
│   ├── alice/               # Alice's files
│   ├── bob/                 # Bob's files
│   └── ...                  # Other users
└── users.dat               # User credentials database
```

---

## Key Technical Details

### Compilation
- **Compiler**: GCC (MinGW on Windows)
- **Libraries**: Winsock2 (Windows Sockets)
- **Build Command**:
  ```bash
  gcc -o server/server.exe server/server.c server/auth.c server/crypto.c server/file_ops.c -lws2_32
  gcc -o client/client.exe client/client.c -lws2_32
  ```

### Message Protocol
Communication uses a simple binary message struct:
```c
typedef struct {
    int command;           // 1-7 (register, login, upload, download, list, delete, logout)
    char username[50];     // Username
    char password[50];     // Password or status response
    char filename[256];    // Filename or file list
    size_t file_size;      // File size
} Message;
```

### Commands (1-7)
- `1` = REGISTER
- `2` = LOGIN  
- `3` = UPLOAD
- `4` = DOWNLOAD
- `5` = LIST
- `6` = DELETE
- `7` = LOGOUT

### Security Notes (Current Implementation)
- **Usernames/Passwords**: Stored in plain text in `users.dat` (simplified for demo)
- **File Storage**: Files not encrypted (simplified for demo)
- **Communication**: Direct socket (not TLS/SSL in Windows version)
- **Access Control**: Each user can only access their own files

### Future Enhancements
For production use, implement:
- OpenSSL integration for file encryption (AES-256)
- TLS/SSL for secure communication
- Password hashing with salt (PBKDF2/bcrypt)
- Multi-threaded server using pthreads
- File access logging and audit trails
- Session tokens/JWT authentication
- File permissions and sharing features

---

## Files Description

### Core Components

**server.c**
- Main server loop accepting client connections
- Command dispatching (register, login, upload, etc.)
- User session management
- File operations coordination

**client.c**
- Interactive command-line interface
- User input parsing
- Network communication with server
- File transmission handling

**auth.c**
- User registration and validation
- Login verification against user database
- User directory creation

**crypto.c**
- Socket message serialization (send/recv)
- File transfer functions
- Basic encryption stubs (ready for AES implementation)

**file_ops.c**
- File upload handling
- File download handling
- File listing
- File deletion

**common.h**
- Shared constants (ports, buffer sizes, timeouts)
- Message structure definition
- Command codes
- Function declarations

---

## Testing the System

### Test Case 1: Basic Registration and Login
```
Server Terminal:
$ server/server.exe
Server listening on port 8080

Client Terminal:
$ client/client.exe
Secure File Storage Client
> register testuser testpass
Response: OK
> login testuser testpass
Logged in
> quit
```

### Test Case 2: File Upload/Download
```
Client Terminal:
> register alice alice123
Response: OK
> login alice alice123
Logged in
> upload myfile.txt
Response: OK
> list
Files:
myfile.txt
> download myfile.txt
Downloaded myfile.txt
> quit
```

### Test Case 3: Multi-User Isolation
```
Client 1:
> register user1 pass1
> login user1 pass1
> upload file1.txt
> list
Files:
file1.txt

Client 2 (create a new client connection):
> register user2 pass2
> login user2 pass2
> list
Files:
(empty - user2 has no files)
```

---

## Troubleshooting

### Port Already in Use
If you get "Bind failed", try:
```bash
# Change PORT in server/common.h from 8080 to another port like 9000
# Recompile: gcc -o server/server.exe ...
```

### Connection Refused
- Ensure server is running in another terminal
- Check if firewall is blocking port 8080
- Verify both are running in the same `CN- project` directory

### Files Not Uploading
- Ensure file exists in the current directory
- Check write permissions on `storage/` directory
- Run client from the project root directory

---

## Command Summary

| Command | Usage | Example |
|---------|-------|---------|
| register | `register <username> <password>` | `register alice pass123` |
| login | `login <username> <password>` | `login alice pass123` |
| upload | `upload <filename>` | `upload mydoc.txt` |
| download | `download <filename>` | `download mydoc.txt` |
| list | `list` | `list` |
| delete | `delete <filename>` | `delete mydoc.txt` |
| logout | `logout` | `logout` |
| quit | `quit` | `quit` |

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    CLIENT PROJECT                       │
│                                                          │
│  ┌─────────────┐         ┌──────────────┐               │
│  │   USER      │         │   COMMAND    │               │
│  │   INPUT     │────────▶│   PARSER     │               │
│  └─────────────┘         └──────────────┘               │
│        ▲                        │                        │
│        │                        ▼                        │
│        │                  ┌──────────────┐               │
│        │                  │   MESSAGE    │               │
│        │                  │    BUILD     │               │
│        │──────────────────└──────────────┘               │
│        │                        │                        │
│        │                        ▼                        │
│        │                  ┌──────────────┐               │
│        │                  │   SOCKET     │               │
│        │                  │    SEND      │               │
│        │                  └──────────────┘               │
│        │                        │                        │
└────────┼────────────────────────┼────────────────────────┘
         │                        │
         │       TCP/IP PORT 8080 │
         │                        ▼
┌────────┼────────────────────────────────────────────────┐
│        │                  SERVER PROJECT                │
│        │                                                │
│        │                  ┌──────────────┐              │
│        │                  │   SOCKET     │              │
│        │                  │   LISTEN     │              │
│        │                  └──────────────┘              │
│        │                        │                      │
│        │                        ▼                      │
│        │                  ┌──────────────┐              │
│        │                  │   MESSAGE    │              │
│        │                  │   RECV       │              │
│        │                  └──────────────┘              │
│        │                        │                      │
│        │                        ▼                      │
│        │                  ┌──────────────┐              │
│        │                  │   COMMAND    │              │
│        │                  │   DISPATCH   │              │
│        │                  └──────────────┘              │
│        │                        │                      │
│        │                        ▼                      │
│        │            ┌─────┬────────┬──────┐            │
│        │            │     │        │      │            │
│        │            ▼     ▼        ▼      ▼            │
│        │         [AUTH] [FILES] [CRYPTO] ...           │
│        │            │     │        │      │            │
│        │            └─────┼────────┼──────┘            │
│        │                  │                            │
│        └──────────────────┼─────────────────┐          │
│                          │                 │          │
│                          ▼                 ▼          │
│                  ┌──────────────┐   ┌──────────────┐  │
│                  │   USER DB    │   │   STORAGE/   │  │
│                  │  users.dat   │   │   username/  │  │
│                  └──────────────┘   └──────────────┘  │
│                         │                 │           │
└─────────────────────────┼─────────────────┼───────────┘
                          │                 │
                    ┌─────▼─────────────────▼──┐
                    │  PERSISTENT DATA STORE   │
                    └──────────────────────────┘
```

---

## Conclusion

The Secure File Storage Server is now **fully operational**! You have a working file storage system that demonstrates:

✅ Client-server architecture  
✅ User authentication  
✅ File storage and management  
✅ Multi-user isolation  
✅ Network communication  
✅ Command-line interface  

The foundation is in place for adding encryption, enhanced security, and additional features as needed for your CSN (Computer Networking) course project.

---

**Created**: March 31, 2026  
**Language**: C  
**Platform**: Windows (MinGW/Winsock2)  
**Status**: Ready for Testing and Demonstration  
