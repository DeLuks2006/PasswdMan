<h1 align="center"> <b>PasswdMan</b> </h1>
<p align="center">A simple, lightweight (and insecure) password manager written in C</p>

## About
PasswdMan is a simple and lightweight password manager written in pure C, leveraging the power of `libsodium` for encryption and `sqlite3` for database management. This project aims to provide a basic, educational example of a password manager.

## Features
- Store and retrieve passwords securely (lie)
- Encrypts passwords using `libsodium`'s AES-256-GCM implementation
- Stores encrypted data in a `sqlite3` database 

## Installation (Linux)

The project should theoretically work on both Windows and Linux however I did not test it yet on Windows.

For Linux:
```bash
# Clone the repository
git clone https://github.com/DeLuks2006/PasswdMan.git
# Navigate to the project directory
cd PasswdMan

# Compile the project
make passwdman
```

## Usage:
```bash
./passwdman
```
## TODO:
- [ ] Change the permissions of the master password hash file to root/admin only

> [!WARNING]
> **PasswdMan should not be used in production!** It is intended for educational purposes only and is not designed to be fully secure.

## Contributing
Contributions are welcome! Please fork the repo and submit a pull request.
