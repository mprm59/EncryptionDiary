# Encryption_Diary
This application alow you to store your private information with a very strong encryption.
I used "GOST 28147-89" encryption.
There are two modes.
1. Encrypting.
  You import a .txt file and selects a password for it. 
  Encrypted document is stored in /archive with a date mark.
2. Viewing.
  All ebcrypted docs are shown in a list. You select one, contents are shown in a box.
  Shown content is a result of decryption with a password which is input in "password" area.
  It means that you will see original text only if you input correct password. Until then you will sea "random" characters.
