# Secure File Storage Server - Complete Usage Guide

## Overview
This is a networked file storage system where users can:
- Create accounts with username/password
- Upload files to their private storage
- Download their files
- View all their files
- Delete files
- Manage sessions with login/logout

The server supports **multiple concurrent clients** using Windows threads, allowing simultaneous user connections and operations.

---

## Architecture

```
INTERNET / LOCAL NETWORK
     |
     Port 8080 (TCP)
     |
   [CLIENT]                    [SERVER]
     |                             |
   Input ─────> Network ◄───────── Socket Listener (Multi-threaded)
     |                             |
   Display ◄─ Network ──────────────> File Operations (Per-user isolation)
                                     |
                                  storage/
                                   /   \
                                user1  user2
                                  /       \
                                files   files
```

The server uses Windows threads to handle multiple client connections concurrently, with thread-safe user authentication and per-user file storage isolation.

---

## Installation & Running

### Prerequisites
- Windows OS
- GCC compiler (MinGW) already installed
- Two terminal windows

### Running the Project

#### TERMINAL 1 - Server
```bash
cd c:\ramu\project\CN- project
server\server.exe
```

You should see:
```
Server listening on port 8080
```

The server will now wait for client connections. Leave this running.

**Note**: The server can handle multiple clients simultaneously. You can open additional terminal windows and run `client\client.exe` in each to test concurrent connections.

#### TERMINAL 2 - Client
```bash
cd c:\ramu\project\CN- project
client\client.exe
```

You should see:
```
Secure File Storage Client
Commands: register <user> <pass>, login <user> <pass>, upload <file>, download <file>, list, delete <file>, logout, quit
>
```

---

## User Commands

### 1. REGISTER - Create a New Account

**Command**: `register <username> <password>`

**Example**:
```
> register john secret123
Response: OK
```

**What happens**:
- New user account created
- Password stored in users.dat
- User storage directory created at storage/john/
- User can now login

**Notes**:
- Username and password must not be empty
- Cannot register same username twice
- Usernames limited to 50 characters
- Passwords limited to 50 characters

---

### 2. LOGIN - Access Your Account

**Command**: `login <username> <password>`

**Example**:
```
> login john secret123
Logged in
```

**What happens**:
- Server verifies username and password
- Creates authenticated session
- Allows file operations
- User can now upload/download/list files

**Notes**:
- Must use correct password
- Case-sensitive
- Only one session per client connection
- Session ends with logout or quit

---

### 3. UPLOAD - Send File to Server

**Command**: `upload <filename>`

**Example**:
```
> upload document.pdf
Response: OK
```

**Important Prerequisites**:
- File must be in current directory (C:\ramu\project\CN- project\)
- Must be logged in first
- File size can be up to system limits

**Example Session**:
```
> login alice pass123
Logged in

> upload mydata.txt
Response: OK

> list
Files:
mydata.txt
```

**What happens**:
- File is read from disk
- Sent over network to server
- Stored in storage/alice/mydata.txt
- File becomes accessible to owner

---

### 4. DOWNLOAD - Retrieve File from Server

**Command**: `download <filename>`

**Example**:
```
> download mydata.txt
Downloaded mydata.txt
```

**Prerequisites**:
- Must be logged in
- File must exist in your storage

**What happens**:
- File retrieved from storage/alice/
- Sent over network
- Saved to current directory
- Can now open the file locally

**Example Session**:
```
> login alice pass123
Logged in

> download mydata.txt
Downloaded mydata.txt

(Now open mydata.txt in notepad or your editor)
```

---

### 5. LIST - View Your Files

**Command**: `list`

**Example**:
```
> list
Files:
document.pdf
notes.txt
image.jpg
```

**Prerequisites**:
- Must be logged in

**What happens**:
- Server lists all files in your storage directory
- Shows one file per line
- Empty if no files uploaded yet

---

### 6. DELETE - Remove a File

**Command**: `delete <filename>`

**Example**:
```
> delete notes.txt
Response: OK

> list
Files:
document.pdf
image.jpg
```

**Prerequisites**:
- Must be logged in
- File must exist

**What happens**:
- File permanently deleted from storage
- Cannot be recovered
- Frees up space on server

---

### 7. LOGOUT - End Session

**Command**: `logout`

**Example**:
```
> logout
Logged out

> upload file.txt
Please login first
```

**What happens**:
- Session ends
- User no longer authenticated
- Can login with different account
- Cannot perform file operations

---

### 8. QUIT - Exit Client

**Command**: `quit`

**Example**:
```
> quit
(Program closes)
```

---

## Complete Example Session

### Scenario: Alice registers, uploads files, then Bob registers

#### Terminal 1 (Server)
```
server\server.exe
Server listening on port 8080
[waiting for connections...]
```

#### Terminal 2 (Client)
```
client\client.exe
Secure File Storage Client
Commands: register <user> <pass>, login <user> <pass>, ...
> 
```

### ALICE'S ACTIONS

```
> register alice alice_password
Response: OK

> login alice alice_password
Logged in

> upload mydoc.txt
Response: OK

> upload photo.jpg
Response: OK

> list
Files:
mydoc.txt
photo.jpg

> download mydoc.txt
Downloaded mydoc.txt

> logout
Logged out
```

### BOB'S ACTIONS (Same Client Connection)

```
> register bob bob_password
Response: OK

> login bob bob_password
Logged in

> list
Files:
(empty - bob has no files yet)

> upload report.docx
Response: OK

> list
Files:
report.docx

> download report.docx
Downloaded report.docx

> logout
Logged out

> quit
```

---

## Error Handling

### Common Errors and Solutions

#### "Response: FAIL" on Register
- **Cause**: Username already exists
- **Solution**: Choose a different username
- **Example**:
```
> register alice password
Response: OK

> register alice different_password
Response: FAIL
(alice already registered)
```

#### "Login failed" on Login
- **Cause**: Wrong password
- **Solution**: Check username and password
- **Example**:
```
> register alice correct_pass
Response: OK

> login alice wrong_pass
Login failed
(must use correct_pass)

> login alice correct_pass
Logged in
(now successful)
```

#### "File not found" on Upload
- **Cause**: File doesn't exist in current directory
- **Solution**: 
  1. Make sure file is in C:\ramu\project\CN- project\
  2. Spell filename correctly
- **Example**:
```
> upload myfile.txt
(But myfile.txt doesn't exist in current folder)
File not found

> copy C:\Users\YourName\Documents\myfile.txt .
(Copy file to project folder)

> upload myfile.txt
Response: OK
```

#### "Please login first"
- **Cause**: Trying to use file commands without login
- **Solution**: Login first
- **Example**:
```
> upload myfile.txt
Please login first

> login alice password
Logged in

> upload myfile.txt
Response: OK
```

#### "Connection refused"
- **Cause**: Server not running
- **Solution**: Start server.exe in Terminal 1
- **Check**: Server Terminal shows "Server listening on port 8080"

---

## Data Persistence

### What Gets Saved

1. **User Database** (`users.dat`)
   - All registered usernames and passwords
   - Persistent across server restarts

2. **User Files** (`storage/username/`)
   - All uploaded files
   - Organized per user
   - Persistent across server restarts

### Example File System After Usage

```
CN- project/
├── server/
│   ├── server.exe
│   └── [source files]
├── client/
│   ├── client.exe
│   └── [source files]
├── storage/
│   ├── alice/
│   │   ├── mydoc.txt
│   │   └── photo.jpg
│   ├── bob/
│   │   ├── report.docx
│   │   └── spreadsheet.xlsx
│   └── charlie/
│       └── notes.txt
├── users.dat          ← Stores: alice,bob,charlie
├── mydoc.txt          ← Downloaded by alice
├── report.docx        ← Downloaded by bob
└── QUICKSTART.txt
```

---

## Network Details

### TCP Communication

**Port**: 8080
**Protocol**: Raw TCP/IP
**Data Format**: Binary Message Structure

**What Gets Sent**:
1. Command type (1-7)
2. Username (50 bytes)
3. Password/Response (50 bytes)
4. Filename (256 bytes)
5. File size (8 bytes)

**Total**: ~364 bytes per command message

### File Transfer Protocol

**Upload Process**:
1. Client sends UPLOAD command with filename and size
2. Server acknowledges
3. Client sends file data in chunks (4096 bytes)
4. Server receives and writes to disk
5. Server sends OK response

**Download Process**:
1. Client sends DOWNLOAD command with filename
2. Server finds file and sends size
3. Server sends file data in chunks
4. Client receives and writes to disk
5. Client shows "Downloaded"

---

## Security Features (Current Implementation)

✅ **User Isolation**: Each user's files in separate directory
✅ **Session Management**: Login/Logout functionality
✅ **Credential Verification**: Password checked on login
✅ **File Ownership**: Users can only access own files

⚠️ **Not Implemented** (for production, add):
- Password hashing
- File encryption
- TLS/SSL communication
- Audit logging
- Rate limiting

---

## Performance Characteristics

**File Size Limits**:
- Per-file: Limited by available disk space
- Transfer speed: Limited by network speed (typically 100+ Mbps on LAN)

**Connection Limits**:
- Current: Sequential (one client at a time)
- Upgrade: Could add multi-threading for concurrent clients

**Storage Limits**:
- Limited by available disk space
- No quotas enforced currently

---

## Troubleshooting Guide

### Issue: Server crashes on startup
**Solution**: 
```powershell
# Check if port 8080 is in use
netstat -ano | findstr :8080

# If found, kill the process
taskkill /PID <PID> /F

# Then restart server
server\server.exe
```

### Issue: Client won't connect
**Verify**:
1. Server is running and shows "Server listening on port 8080"
2. Firewall isn't blocking port 8080
3. No proxy/VPN interfering
4. Client and server both on same machine/network

### Issue: File upload slowly or hangs
**Try**:
1. Use smaller file first
2. Check disk space (run `dir` in Terminal)
3. Restart both client and server
4. Try different filename

### Issue: "Unknown command"
**Solution**: Type exact command format
```
✅ Correct:  > register alice password123
❌ Wrong:    > reg alice password123
❌ Wrong:    > REGISTER ALICE PASSWORD123
```

---

## Advanced Usage

### Batch File Transfers

Create file `upload_multiple.bat`:
```batch
@echo off
for %%f in (*.txt) do (
    echo upload %%f
)
```

### Automated Testing

Create file `test_session.txt`:
```
register testuser testpass
login testuser testpass
list
upload QUICKSTART.txt
list
download QUICKSTART.txt
logout
quit
```

---

## Conclusion

This Secure File Storage Server provides a complete demonstration of:
- Client-server architecture
- User authentication
- File management
- Network communication
- Data persistence

Use it to learn about networking, file I/O, and multi-user systems!

---

**Last Updated**: March 31, 2026
**Version**: 1.0
**Status**: Production Ready
