% Binary protocol for communication between instances of HaveClip

Architecture
------------
HaveClip defines conversations and messages (commands).

Messages are generic means of exchanging information while conversation
brings them into context. Messages may be used in any order
and by any conversation.

A conversation has defined structure of communication by a chain
of messages in either receiving or sending role (message receives
or transmits data).

A conversation may change the order of messages at runtime, based
on peer's responses or other parameters. A conversation may also
morph into another conversation. However, it must always do so on both
peers, otherwise communication will fail.

Authentication
--------------
Conversation may choose what level of authentication should be applied.

Authentication is enforced after SSL/TLS handshake and after it
is clear what conversation will take place, so that the conversation
level of authentication may be considered.

The levels are:

 - NoAuth - no authentication at all, used for [Introduction](#introduction)
 - Introduced - the peer must be introduced, used for [Verification](#verification)
 - Verified - the peer must be verified, meaning it is in pool, has valid
	      certificate and it matches

Possible conversations
----------------------

### Introduction
Used when adding a node to the pool. Target node should open a dialog
where user can type the security code shown on requestor.

The requestor should also remember target node certificate and check
it during verification.

  1. Request: [Introduce](#introduce)
  2. Reply: [Introduce](#introduce)

### Verification
Follows introduction, target node sends the security code back
to requestor. If the code is correct, devices are paired.

There are three tries after which security code becomes invalid
and pairing failed.

  1. Request: [SecurityCode](#securitycode)
  2. Reply: [Confirm](#confirm)

### Clipboard update

Requestor is sending ClipboardItem to replier.

  1. Request: [ClipboardUpdateReady](#clipboardupdateready)
  2. Reply: [ClipboardUpdateConfirm](#clipboardupdateconfirm)
  3. Request: [ClipboardUpdateSend](#clipboardupdatesend)
  4. Reply: [Confirm](#confirm)


Protocol structure
------------------

### Message header

  Length    Type         Meaning
 -------    -------      ------------------------------------------------
       4    uint32       magic number, identifies correct data format
       4    int32        version
       4    int32        conversation
       4    int32        message type
       8    quint64      message length
       -    QString      password

### Message types

#### Ping
Contains no data, sends just header.

#### Introduce

  Length    Type         Meaning
 -------    -------      ------------------------------------------------
       -    QString      computer name
       2    quint16      port HaveClip listens on

#### SecurityCode
  Length    Type         Meaning
 -------    -------      -----------------------------------------------
       -    QString      security code

#### ClipboardUpdateReady

  Length    Type         Meaning
 -------    -------      ------------------------------------------------
       4    int32        mode (clipboard, selection, both)

#### ClipboardUpdateConfirm

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            status (continue/abort)
       4    int8             meaning of the list below (Accept or Exclude)
       -    QStringList      list of mime types

#### ClipboardUpdateSend

  Length    Type                           Meaning
 -------    -------------------------      ------------------------------------------------
       4    int32                          mode (clipboard, selection, both)
       -    QStringList                    formats
       -    QByteArray * formats.size      mime data

#### Confirm

  Length    Type             Meaning
 -------    -----------      -----------------------------------------------------------
       4    int32            status (ok, abort, not exists, not matches, not understood)
