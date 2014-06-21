% Binary protocol for communication between instances of HaveClip

Possible conversations
----------------------

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
