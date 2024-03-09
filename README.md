<h1 align="center"> <b>PasswdMan</b> </h1>
<p align="center">Simple password manager made in C/C++.</p>
<p align="center">
  <code>(current state)</code>
</p>

<p align="center">
  <img src="PASSWDMAN-Curent-State.png">
</p>

## Progress Updates
**Working on the GUI rn...** (and the password generation)

gotta learn to use libsodium and sqlite3 libraries too :P

## 
**⚠️ TODO: ⚠️**
<!-- Some Goals for the Project. -->
- [ ] Password generation (50% done)
- [ ] Password storing
- [ ] Encryption
- [ ] GUI using imGUI (I'm workin on it!!)
- [ ] Input Validation
- [ ] Master Password
## 
**Brainstorming:**

(stuff here isn't thought completely trough yet)

*Saving created buttons/entries:*
1. store data in SQL table
2. grab data from the table at startup
3. use grabbed data to initialize structs
4. use initialized structs to create buttons that when clicked render the needed UI elements

passwords will be stored encrypted and will only be decrypted when needed.

the master passwords hash will only be saved and not the password itself (for obvious reasons)
